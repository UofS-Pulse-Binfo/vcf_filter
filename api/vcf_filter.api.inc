<?php
/**
 * @file
 * API Functions for this module.
 */

/**
 * Retrieve all VCF files.
 *
 * @return
 *   An array of VCF file details, keyed by vcf_file_id.
 */
function vcf_filter_get_vcf_files() {
  global $user;

  // Gran the roles and user id of the current user.
  $uid = $user->uid;
  $rids = is_array($user->roles) ? array_keys($user->roles) : array();

  // Only grab files that the current user has permission to see.
  $files = db_query('
    SELECT f.*
    FROM {vcf_files_perm} perm
    LEFT JOIN {vcf_files} f ON f.vcf_file_id=perm.vcf_file_id
    WHERE perm.uid=:uid OR perm.rid IN (:rids)',
    array(':uid' => $uid, ':rids' => $rids))->fetchAllAssoc('vcf_file_id', PDO::FETCH_ASSOC);

  return $files;
}

/**
 * Retrieve details for a given VCF file.
 *
 * @param $vcf_file_id
 *   The primary key for the VCF File.
 * @return
 *   An object describing the VCF file.
 */
function vcf_filter_get_vcf_file($vcf_file_id) {

  $file = db_query('SELECT * FROM {vcf_files} WHERE vcf_file_id=:id',
    array(':id' => $vcf_file_id))->fetchObject();

  return $file;
}

/**
 * Filter a VCF file using VCFtools.
 *
 * MUST BE RUN ON THE COMMAND-LINE VIA DRUSH.
 *
 * @param $file_path
 *   The absolute path to the VCF file to be filtered.
 * @param $filter_criteria
 *   An array of filter criteria to be performed on the above VCF file.
 * @return
 *   The full patch of the filtered VCF file.
 */
function vcf_filter_filter_file($file_path, $filter_criteria, $drush_use_limited = false) {
  $new_file = variable_get('file_temporary_path') . '/' . uniqid();

  if ($drush_use_limited === false){
    drush_log('NOTE: We are not using the PL at this point to specify the genotypes.', 'warning');
  }
  else {
    print "NOTE: We are not using the PL at this point to specify the genotypes.\n";
  }


  $command = 'vcftools --vcf ' . $file_path . ' --recode --out ' . $new_file;

  //restrict_dataset:germplasm
  //1.need to read GERMPLASM array from $filter_criteria, and save it into a file
  //2.use the file in vcftools with command --keep
  
  $filter_criteria_germplasm_temp = variable_get('file_temporary_path') . '/' . 'germplasm.' . uniqid() . '.txt';
  
  if (!empty($filter_criteria['restrict_dataset']['germplasm_select'])){
    
    $handle_out = fopen($filter_criteria_germplasm_temp,'w') or die ("\nCannot create file: $filter_criteria_germplasm_temp .\n");
    fwrite($handle_out, $filter_criteria['restrict_dataset']['germplasm_select']);
    
    $command .= ' --keep ' .  $filter_criteria_germplasm_temp;
  }
  
  //restrict_dataset:regions
  //1.need to read REGIONS array from $filter_criteria, parse each line, and save it into a file in bed format
  //2.use the file in vcftools with command --bed
  
  $filter_criteria_regions_temp = variable_get('file_temporary_path') . '/' . 'regions.' . uniqid() . '.bed';
  
  if (!empty($filter_criteria['restrict_dataset']['regions'])){
    $handle_out = fopen($filter_criteria_regions_temp,'w') or die ("\nCannot create file: $filter_criteria_regions_temp .\n");
    fwrite($handle_out, ('#Filter by regions' . "\n"));
    
    $one_value_regions = explode("\n", $filter_criteria['restrict_dataset']['regions']);
    
    foreach($one_value_regions as $value_regions){
      if (preg_match('/^#/', $value_regions)){
        continue;
      }
      
      $fc_rd_regions_chrom = explode(":", $value_regions);
      fwrite($handle_out, ($fc_rd_regions_chrom[0] . "\t"));
      
      $fc_rd_regions_position = explode("..", $fc_rd_regions_chrom[1]);
      fwrite($handle_out, ($fc_rd_regions_position[0] . "\t" . $fc_rd_regions_position[1] . "\n"));
      
    }
    $command .= ' --bed ' .  $filter_criteria_regions_temp;
  }


  // Bi-allelic:
  if ($filter_criteria['bi-allelic'] == 1) {
    $command .= ' --min-alleles 2 --max-alleles 2';
  }

  // Minimum SNP Call Read Depth:
  if (!empty($filter_criteria['call_read_depth'])) {
    $command .= ' --minDP ' . escapeshellarg($filter_criteria['call_read_depth']);
  }

  // Minor Allele Frequency:
  if (!empty($filter_criteria['minor_allele_freq'])) {
    // NOTE: We allow the user to provide the frequency as an integer 0-100;
    // however, vcftools expects a float between 0-1.
    $minor_allele_freq = $filter_criteria['minor_allele_freq'] / 100;
    $command .= ' --maf ' . escapeshellarg($minor_allele_freq);
  }

  // Maximum Missing Count:
  if (!empty($filter_criteria['max_missing_count'])) {
    $command .= ' --max-missing-count ' . escapeshellarg($filter_criteria['max_missing_count']);
  }

  // Maximum Missing Count:
  if (!empty($filter_criteria['max_missing_freq'])) {
    // NOTE: We allow the user to provide the frequency as an integer 0-100;
    // however, vcftools expects a float between 0-1.
    $max_missing_freq = 1 - ($filter_criteria['max_missing_freq'] / 100);
    $command .= ' --max-missing ' . escapeshellarg($max_missing_freq);
  }

  // Execute the command.
  //-------------------------
  $outfile = $new_file . '.recode.vcf';
  print 'Filter results will be in: '.$outfile."\n";
  print str_repeat('-', 60)."\n";
  $output = shell_exec($command);

  print "\n" . $command . "\n";

  // Print the output from the command.
  print $output."\n";
  print str_repeat('-', 60)."\n";

  // Check for error.
  if (!file_exists($outfile)) {
    if ($drush_use_limited === false){
      return drush_set_error('ERROR: Filtering of the VCF file was not successful.');
    }
    else {
      print "ERROR: Sorting of the VCF file was not successful.\n";
      return false;
    }
  }
  
  if (file_exists($filter_criteria_germplasm_temp)){
    unlink($filter_criteria_germplasm_temp);
  }
  
  if (file_exists($filter_criteria_regions_temp)){
    unlink($filter_criteria_regions_temp);
  }
  
  return $outfile;
}

/*
 * Tassel expects the SNPs in input files in a specific order. Our vcf file is not in tasel's order now.
 * The file can be sorted using SortGenotypeFilePlugin provided by Tassel, but it will be nice if the file
 * is ordered and skip this step for users.
 * @param $vcf_file_unsorted
 *   the vcf file generated by function vcf_filter_filter_file
 * @returen $new_file
 *   sorted vcf format file so different export format files can be used in tassel directly
*/

function vcf_filter_sort_filtered_file($vcf_file_unsorted, $drush_use_limited = false) {
  $new_file = variable_get('file_temporary_path') . '/' . uniqid();
  $new_file = $new_file . '.filtered.sorted.vcf';

  // execute the command
  // to count number of lines of header (lines start with #)
  $command = 'grep "#" '.$vcf_file_unsorted.' | wc -l';
  $number_line_header = shell_exec($command);

  // first, print headers to new file
  // second, sort the rest first by snp name, then by numerical order of position
  $command = 'head -' . intval($number_line_header) . ' ' . $vcf_file_unsorted . ' > ' . $new_file;
  $command .= ' | tail -n+$((' . intval($number_line_header) . '+1)) ' . $vcf_file_unsorted;
  $command .= ' | sort -k1,1 -k2,2n >> ' . $new_file;

  // execute the command
  //-----------------------------
  print 'Sorted vcf file will be in: '.$new_file."\n";
  shell_exec($command);

  // Print the output from the command.
  print $new_file."\n";
  print str_repeat('-', 60)."\n";

  // Check for error.
  if (!file_exists($new_file)) {
    if ($drush_use_limited === false){
      return drush_set_error('ERROR: Sorting of the VCF file was not successful.');
    }
    else {
      print "ERROR: Sorting of the VCF file was not successful.\n";
      return false;
    }
  }
  return $new_file;
}

/*
 * Processes a VCF file into an associative array for ease of manipulation for each SNP in the file.
 * This function only processes a single line in a VCF at a time, thus it is designed to be used within
 * a loop iterating through a VCF file. This is designed primarily due to the extreme size of some
 * VCF files!
 *
 * @param $VCF_header
 *   An array or tab-delimited string representing the VCF header from the same file for which a
 *   line is being processed.
 *   The header consists of 9 columns describing chromosome, position, ID, reference allele, alternate
 *   allele, etc. in addition to any number of columns representing each sample for which a genotype
 *   is recorded for that site.
 * @param $VCF_line
 *   A line in VCF format that represents a single site in a genome and describes variant information.
 * @return
 *   An associative array representing a specific LINE in a VCF file.
 */

function vcf_filter_read_VCF_into_array($VCF_header, $VCF_line, $drush_use_limited = false) {

  // Validate the header is in the correct format
  if (is_array($VCF_header)) {
    if (strcmp($VCF_header[0], '#CHROM') !== 0) {
      if ($drush_use_limited === false){
        return drush_set_error('ERROR: Expected first element in VCF header to contain: #CHROM');
      }
      else {
        print "ERROR: Expected first element in VCF header to contain: #CHROM\n";
        return false;
      }
    }
    else if (sizeof($VCF_header) < 10) {
      die ("ERROR: The size of VCF header is too small to be valid.\n");
    }
  }
  else {
    if (preg_match("/^#CHROM\t/", $VCF_header)) {
      $header = explode("\t", $VCF_header);
    }
    else {
      if ($drush_use_limited === false){
        return drush_set_error('ERROR: Expected VCF header to begin with: #CHROM');
      }
      else{
        print "ERROR: Expected VCF header to begin with: #CHROM\n";
        return false;
      }
    }
  }

  // Validate the line from the VCF follows the spec as best as we can
  // @TODO: Check for proper FORMAT? POS is an integer?
  if (!is_array($VCF_line)) {
    if (preg_match("/\t/", $VCF_line)) {
      $line = explode("\t", $VCF_line);
      $VCF_line = $line;
    }
    else {
      if ($drush_use_limited === false){
        return drush_set_error('ERROR: Expected VCF line to be an array or tab-delimited!');
      }
      else {
        print "ERROR: Expected VCF line to be an array or tab-delimited\n";
        return false;
      }
    }
  }

  // Create an associative array using the header as the keys
  if (count($VCF_header) === count($VCF_line)) {
    $marker = array_combine($VCF_header,$VCF_line);
  }
  else {
    if ($drush_use_limited === false){
      return drush_set_error('ERROR: The provided VCF header and VCF line do not contain the same number of columns.');
    }
    else {
      print "ERROR: The provided VCF header and VCF line do not contain the same number of columns.\n";
      return false;
    }
  }

  // Explode the FORMAT cell to find out if/where PL exists
  $format_keys = explode(':',$marker['FORMAT']);
  $changePL = 0;

  // For each genotype source, explode the format fields so that we can pull out the genotype directly
  $num_col = count($VCF_header);
  for ($i=9;$i<$num_col;$i++) {

    $source_name = $VCF_header[$i];

    if (is_array($format_keys)) {
      $num_fk = count($format_keys);
      $format_fields = explode(":",$marker[$source_name]);
      $num_ff = count($format_fields);
      // NOTE: Why the following comparison between format key and field counts?
      // Occasionally we may see that the format field will specify additional sub-fields than what is
      // actually present for that source. This is because of questionably loose requirements for VCF:
      // missing sub-fields are shown as ".", except for trailing fields which can be dropped. Therefore,
      // if there are fewer format fields for a source than expected, we will add onto the end of the
      // array the appropriate number of missing values.
      while ($num_fk > $num_ff)
      {
        //array_push($format_fields,".");
        $num_ff = count($format_fields);
      }
      $geno_field_arr = array_combine($format_keys,$format_fields);
      // Now we nest the marker array with this new genotype fields array for each source
      // Example: Marker -> Source1 -> GT -> 0/1
      //                            -> DP -> 25
      $marker[$source_name] = $geno_field_arr;

      // Check if we have the PL field, and change GT accordingly if necessary
      // Pick a ridiculously high index in case there is no minimum PL
      $call_index = '99';

      if ($marker[$source_name]['GT'] !== './.') {
        $phred_likelihood = explode(',',$marker[$source_name]['PL']);
        for ($j=0; $j < count($phred_likelihood); $j++) {
          if ($phred_likelihood[$j] === '0') {
            if ($j === 0) {
              $call_index = 'zero';
            }
            else {
              $call_index = $j;
            }
          }
        }
        if ($call_index !== '99') {
          switch ($call_index) {
            case 'zero':	$genotype = '0/0'; break;
            case 1:	$genotype = '0/1'; break;
            case 2: $genotype = '1/1'; break;
            case 3:	$genotype = '0/2'; break;
            case 4:	$genotype = '1/2'; break;
            case 5: $genotype = '2/2'; break;
            // Can't read beyond this for PL, set to missing :-(
            default: $genotype = './.'; break;
          }
        // No PL score of 0 found, strange, set the genotype call to missing.
        }
        else { $genotype = './.'; }
        $oldcall = $marker[$source_name]['GT'];
        if (strcmp($genotype, $oldcall) !== 0 ) {
          $marker[$source_name]['GT'] = $genotype;
          $changePL++;
        } //else { print "No change \t"; }
      }
    }
    else { // The FORMAT column could not be split, thus it must only contain 'GT'
      $genotype_field = [];
      $genotype_field[$marker['FORMAT']] = $marker[$source_name];
      $marker[$source_name] = $genotype_field;
    }
  }
  //print "Changed PL " . $changePL . " times.\n";
  return $marker;
}

/**
 * A simple function to move an element in an array to be the first element. All other elements
 * will be shifted accordingly.
 *
 * @param &$array
 *   An array you want to re-arrange
 * @param $str
 *   The element in the array that you want to move to the 0 index
 * @return
 *   The same array post re-arrangement (it was passed in as a reference)
 */

function moveToFirst(&$array, $str)
{
  $key=array_search($str,$array);
  if($key) unset($array[$key]);
  array_unshift($array,$str);
  return $array;
}

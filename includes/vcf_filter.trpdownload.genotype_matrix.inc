<?php
/**
 * @file
 * Handles download of VCF files in Genotype Matrix Format.
 */

/**
 * Generate Genotype Matrix File from VCF.
 *
 * @param $variables
 *   An associative array of parameters including:
 *     - q: all the query paramters.
 *     - site_safe_name: a sanitized version of your site name for use in variables & filenames.
 *     - type_info: an array of info for the download type.
 *     - suffix: the file format suffix.
 *     - filename: the filename of the file to generate not including path.
 *     - fullpath: the full path and filename of the file to generate.
 *     - format_name: a human-readable description of the format.
 * @param $job_id
 *   The ID of the tripal job executing this function ;-).
 * @param $drush_use_limited
 *   are drush function limited
 */
function vcf_filter_genotype_matrix_generate_file($variables, $job_id = NULL, $drush_use_limited = false) {
  // Create the file and ready it for writting to.
  $filepath = $variables['fullpath'] . $variables['filename'];
  print "File: ".$filepath."\n";
  // Get the VCF file details.
  $vcf_file = vcf_filter_get_vcf_file($variables['q']['vcf_file_id']);
  print "\nVCF File (".$vcf_file->name."): ".$vcf_file->file_path."\n";
  // Check the input file still exists.
  if (!file_exists($vcf_file->file_path)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Original VCF file not found!");
      exit(2);
    }
    else{
      print "ERROR: Original VCF file not found!\n";
      exit(2);
    }
  }
  // Print out parameters.
  print "Parameters:\n";
  foreach ($variables['q'] as $label => $value) {
    print '  ' . $label . ': ' . $value."\n";
  }
  // Give the user some indication we've done something...
  tripal_set_job_progress($job_id, 5);
  // Using the parameters user gave to filter vcf file
  //=====================================================
  print "\nFiltering the Original VCF\n";
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q'], $drush_use_limited);
  if (!$filtered_vcf) {
    exit(3);
  }
  // Sort filtered vcf file for use in Tassel
  print "\nSorting the filtered VCF\n";
  $filtered_sorted_vcf = vcf_filter_sort_filtered_file($filtered_vcf, $drush_use_limited);
  if (!$filtered_sorted_vcf) {
    exit(4);
  }

  // Half way point...
  tripal_set_job_progress($job_id, 50);
  // Ready for next step: FORMAT.
  // Convert filtered vcf file to format user expected (genotype matrix in this case).
  //=====================================================
  //------------------------------------------------------------------
  // new script starts here, above codes are copied from old script with minor changes
  // the extension of GenotypeMatrix format filename can to be '.matrix.tsv'
  print "\nConversion from VCF to Genotype Matrix Format.\n";
  $format_tmp = $variables['fullpath'] . uniqid();

  $outfile = $format_tmp . '.matrix.tsv';
  print 'Converted results will be in: '.$outfile."\n";
  print str_repeat('-', 60)."\n";

  //use function converting filter vcf filt to GenotypeMatrix format
  vcf_filter_convert_VCF_to_GenotypeMatrix($filtered_sorted_vcf, $outfile, $drush_use_limited);

  if (!file_exists($outfile)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Unable to convert VCF file to Genotype Matrix format.");
      exit(5); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
    }else{
      print "ERROR: Unable to convert VCF file to Genotype Matrix format.\n";
      exit(5);
    }
  }

  // Tell the user we're almost there!
  tripal_set_job_progress($job_id, 90);

  print "\nMove Formatted file to the name we expect.\n";
  shell_exec("mv $format_tmp.matrix.tsv $filepath");
  if (!file_exists($filepath)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Unable to move GenotypeMatrix file into expected file director: $filepath");
      exit(6); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
    }else{
      print "ERROR: Unable to move GenotypeMatrix file into expected file director: $filepath\n";
      exit(6);
    }
  }

  // Done!
  tripal_set_job_progress($job_id, 100);

}

/**
 * This script will take any spec-conforming VCF file and convert it to a GenotypeMatrix format file
 *
 *
 *  @param $in_vcf_filename
 *    A tab-delimited text file in Variant Call Format (VCF). The spec for VCF files can be found here:
 *    http://samtools.github.io/hts-specs/
 *  @param $out_GenotypeMatrix_filename
 *    A name to give the final GenotypeMatrix format output file.
*/

function vcf_filter_convert_VCF_to_GenotypeMatrix($in_vcf_filename, $out_GenotypeMatrix_filename) {

  // create output file
  $handle_out = fopen($out_GenotypeMatrix_filename,'w') or die ("\nCannot create file: $out_GenotypeMatrix_filename .\n");
  $read_header = TRUE;

  // read vcf file by lines
  foreach (new SplFileObject($in_vcf_filename) as $line) {
    $line = trim($line);
    // if one line starts with '#CHROM' => header
    // if one line doesn't start with '#' and not empty => one SNP line
    if (preg_match('/^#CHROM/', $line)) {
      $headerline = explode("\t", $line);
      fwrite($handle_out, ("Name\tBackbone\tPosition"));
      for ($i = 9; $i < count($headerline); $i++) {
        // tassel can not sepeare tabs and spaces in germplasm names, so we have to replace spaces with underscores for tassel
        // only for GenotypeMatrix, the problem doesn't exit for vcf files
        $headerline[$i] = str_replace(' ', '_', $headerline[$i]);
        fwrite($handle_out, "\t$headerline[$i]");
      }
    }

    if ((!preg_match('/^#/', $line)) && (!empty($line))) {

      // Once we are done reading the header line, we need to start a new lines
      // in the output file.
      if ($read_header == TRUE) {
        $read_header = FALSE;
        fwrite($handle_out, "\n");
      }
      $marker = vcf_filter_read_VCF_into_array($headerline, $line);

      //  GenotypeMatrix format: Name Backbone Position (Germplasm name)
      $marker_name = $marker['#CHROM'] . 'p' . $marker['POS'];
      fwrite($handle_out, ($marker_name . "\t" . $marker['#CHROM']. "\t" . $marker['POS']));

      // merge the reference and alternates(1-3) genotypes into one array $gt_ref_min
      // corresponding $gt_ref_min[0] for reference gt, $gt_ref_min[1] for first alternate and so on
      $gt_ref_min = array_merge(array($marker['REF']), explode(',',$marker['ALT']));

      // 2 loops: first by samples (9th column to end)
      // second by number of GT(genotype)(e.g.:0/0) in FORMAT(e.g.: 0/0:0,36,89:51:5:40) of vcf file
      // at first explod 0/0:0,36,89:51:5:40 into array("0/0","0,36,89",...,"40") then explode first element into new array("0", "0")
      // then match "0" for reference gt, "1" for first alternate and so on

      for ($m = 9; $m < count($headerline); $m++) {
        $current_sample = $headerline[$m];

        // should explode both by '/' and '|' for both unpashed and pahsed
        $call_gt = explode('/', str_replace("|","/",$marker[$current_sample]['GT']));
        fwrite($handle_out, "\t");

        // check the number of genotypes exits
        // for genotypes:  .|. or 0|. or 1|. vcftools will convert it to . ; 0; 1 after filtering
        // even the gt should not exist, but it could happen accidently
        // it'll be the best to check this, so we can be alerted when it actually happen
        if (count($call_gt) <= 1){
          $germ_w_error = $m-9;
          print "ERROR: Only one genotype showed, might be caused by vcftools processing improper phased genotypes for "."$germ_w_error"."th  germplasm in line:"."\n"."$line.\n";
        }

        $gt_print;
        $slash_required = FALSE;
        for ($n=0; $n < count($call_gt); $n++) {
          if (strcmp($call_gt[$n], ".") == 0) {
            //fwrite($handle_out, 'N');
            $gt_print[$n] = 'N';
          }
          elseif (is_numeric($call_gt[$n]) == true) {
            //fwrite($handle_out, $gt_ref_min[$call_gt[$n]]);
            $gt_print[$n] = $gt_ref_min[$call_gt[$n]];
            if (strlen($gt_ref_min[$call_gt[$n]]) > 1){
              $slash_required = TRUE;
            }
          }
          else {
            die ("ERROR: invalid genotype call at marker position ". $marker['#CHROM']. " ". $marker['POS']. "\n");
          }
        }

        for ($n=0; $n < count($gt_print); $n++) {
          if ( ($n != 0) && ($slash_required == TRUE) ){
            fwrite($handle_out, '/');
          }
          fwrite($handle_out, $gt_print[$n]);
        }

      // end for loop through germplasm
      }

      fwrite($handle_out, ("\n"));
    }

  }
  fclose($handle_out);
  unlink($in_vcf_filename);
}

<?php
/**
 * @file
 * Handles VCF downloads in A/B Biparental Format.
 */

/**
 * Generate ABH File from VCF.
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
 */
function vcf_filter_abh_generate_file($variables, $job_id = NULL) {

  // Create the file and ready it for writing to.
  $filepath = variable_get('trpdownload_fullpath', '') . $variables['filename'];
  drush_print("File: " . $filepath);
  //$FILE = fopen($filepath, 'w') or die ('Unable to create file to write to');

  // Get the VCF file details.
  $vcf_file = vcf_filter_get_vcf_file($variables['q']['vcf_file_id']);
  drush_print("\nVCF File (".$vcf_file->name."): ".$vcf_file->file_path);

  // Check the input file still exists.
  if (!file_exists($vcf_file->file_path)) {
    drush_set_error("ERROR: Original VCF file not found!");
    exit(2);
  }

  // Print out parameters.
  drush_print("Parameters:");
  foreach ($variables['q'] as $label => $value) {
    drush_print('  ' . $label . ': ' . $value);
  }

  // Give the user some indication we've done something...
  tripal_set_job_progress($job_id, 5);

  // Filter
  //================
  drush_print("\nFiltering the Original VCF");
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q']);
  if (!$filtered_vcf) {
    exit(3);
  }

  // Almost there!
  tripal_set_job_progress($job_id, 50);

  // Format
  //================
  // @TODO: Pull out the parents by getting the first two samples in the VCF header.
  // NOTE: Obviously, we should not be making this assumption, but it works for now as
  // a workaround.

  $vcf_header = shell_exec("grep \"^#CHROM\" $filtered_vcf");
  $vcf_header_array = explode("\t", $vcf_header);
  $maternal_parent = $vcf_header_array[9];
  $paternal_parent = $vcf_header_array[10];

  drush_print("\nConversion from VCF to A/B Biparental Format.");
  drush_print("USING THE FOLLOWING AS THE MATERNAL PARENT: " . $maternal_parent);
  drush_print("USING THE FOLLOWING AS THE PATERNAL PARENT: " . $paternal_parent);
  $converted_ABH = vcf_filter_convert_VCF_to_ABH($filtered_vcf, $maternal_parent, $paternal_parent, $filepath);
  if (!$converted_ABH) {
    exit(3);
  }

  // Done!
  tripal_set_job_progress($job_id, 100);
}


/**
 * This script will take any spec-conforming VCF file and convert it to a standard ABH format for use
 * with mapping programs.
 *
 * NOTE: It is highly recommended that the VCF file is "sanitized", which ensures the GT field agrees
 *       with the PL field. This function ONLY checks the GT field.
 *
 *  @param $input_file
 *    A tab-delimited text file in Variant Call Format (VCF). The spec for VCF files can be found here:
 *    http://samtools.github.io/hts-specs/
 *  @param $maternal_parent
 *    The name of the sample which is the maternal parent for this population. The name must match exactly
 *    with the name within the VCF file.
 *  @param $paternal_parent
 *    The name of the sample which is the paternal parent for this population. The name must match exactly
 *    with the name within the VCF file.
 *  @param $output_file
 *    A name to give the final ABH format output file.
 */

function vcf_filter_convert_VCF_to_ABH($input_file, $maternal_parent, $paternal_parent, $output_file) {

  // ----- FILE VALIDATION -----
  $input_file = trim($input_file);
  if (!$input_file) { return drush_set_error(dt('ERROR: Unable to find input file: @input_file!', array('@input_file' => $input_file))); }
  if ($output_file == NULL) { return drush_set_error(dt('ERROR: A name for your output file is required!')); }

  // Open our files
  $INFILE = fopen($input_file, 'r') or die ("ERROR: Unable to open $input_file!\n");
  $OUTFILE = fopen($output_file, 'w') or die ("ERROR: Unable to create $output_file!\n");

  print "Starting Conversion.\n";
  // ----- PROCESS -----
  // First deal with headers and save the column names
  while(!feof($INFILE)) {
    $current_line = fgets($INFILE);
    if (empty($current_line)) continue;

    // Check for the header lines
    if (preg_match('/^##/', $current_line)) { continue; }
    if (preg_match('/^#CHROM/', $current_line)) {
      $current_line = trim($current_line);
      $header = explode("\t",$current_line);
      break;
    }
  }

  // Check that the user-provided maternal and paternal parents exist in the file.
  $sample_names = array_slice($header, 9);
  //print_r($sample_names);
  if (!in_array($maternal_parent, $sample_names)) {
    return drush_set_error(dt('ERROR: Could not find a match for @maternal in your input file (@input_file).', array('@maternal' => $maternal_parent, '@input_file' => $input_file)));
  }
  if (!in_array($paternal_parent, $sample_names)) {
    return drush_set_error(dt('ERROR: Could not find a match for @paternal in your input file (@input_file).', array('@paternal' => $paternal_parent, '@input_file' => $input_file)));
  }

  // First move the paternal parent, then the maternal, to ensure maternal is 1st and paternal is 2nd
  moveToFirst($sample_names, $paternal_parent);
  moveToFirst($sample_names, $maternal_parent);

  $new_header = array_merge(array("Chromosome", "Position"), $sample_names);
  fputcsv($OUTFILE, $new_header, "\t");

  // Now iterate through all the SNPs
  while (!feof($INFILE)) {

    $current_line = fgetcsv($INFILE, 0, "\t");
    if (empty($current_line)) continue;

    $marker = vcf_filter_read_VCF_into_array($header, $current_line);

    // Save the chromosome and position
    $chrom = $marker['#CHROM'];
    $pos = $marker['POS'];

    $converted_line = array($chrom, $pos);

    foreach ($sample_names as $current_sample) {

      // Genotype calls are in the format 0/1 or 0|1, so split the alleles apart
      $genotype_calls = $marker[$current_sample]['GT'];
      if (preg_match('/\//', $genotype_calls)) {
        $marker[$current_sample]['GT'] = explode('/', $genotype_calls);
      }
      else if (preg_match('/|/', $genotype_calls)) {
        $marker[$current_sample]['GT'] = explode('|', $genotype_calls);
      }
      // Now the calls are in an array
      $genotype_calls = $marker[$current_sample]['GT'];

      if (strcmp($current_sample, $maternal_parent) === 0) {
        $maternal_genotype = $genotype_calls;
        array_push($converted_line, "A");
        continue;
      } else if (strcmp($current_sample, $paternal_parent) === 0) {
        $paternal_genotype = $genotype_calls;
        array_push($converted_line, "B");
        continue;
      }

      // Translate the alleles to:
      //  A if it matches the maternal parent
      //  B if it matches the paternal parent
      //  H if it is heterozygous
      //  - if it is missing
      if ($genotype_calls[0] == $maternal_genotype[0]) {
        if ($genotype_calls[1] == $maternal_genotype[1]) {
          array_push($converted_line, "A");
        } else if ($genotype_calls[1] == $paternal_genotype[1]) {
          array_push($converted_line, "H");
        } else { array_push($converted_line, "-"); }
      } else if ($genotype_calls[0] == $paternal_genotype[0]) {
        if ($genotype_calls[1] == $paternal_genotype[1]) {
          array_push($converted_line, "B");
        } else if ($genotype_calls[1] == $maternal_genotype[1]) {
          array_push($converted_line, "H");
        } else { array_push($converted_line, "-"); }
      } else { array_push($converted_line, "-"); }
    }

    // And print it to the output file!
    fputcsv($OUTFILE, $converted_line, "\t");

  }
  print "Finished Conversion.\n";

  fclose($INFILE);
  fclose($OUTFILE);

  return true;
}

<?php
/**
 * @file
 * Handles basic download of a VCF file.
 */

/**
 * Generate Filtered VCF File from VCF.
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
 * @param $test_no_drush
 *   is this function is used for UnitTest ;-).
 */
function vcf_filter_vcf_generate_file($variables, $job_id = NULL, $drush_use_limited = false) {

  print_r($variables);
  print "\n";

  // Create the file and ready it for writting to.
  $filepath = variable_get('trpdownload_fullpath', '') . $variables['filename'];
  print "Writing into file: ".$filepath."\n";


  // Get the VCF file details.
  $vcf_file = vcf_filter_get_vcf_file($variables['q']['vcf_file_id']);
  print "\nVCF File (".$vcf_file->name."): ".$vcf_file->file_path."\n";
  print_r($vcf_file);

  // Check the input file still exists.
  if (!file_exists($vcf_file->file_path)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Original VCF file not found!");
      exit(2);
    }else{
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

  // Filter.
  //===============
  print "\nFiltering the Original VCF\n";
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q'], $drush_use_limited);
  if (!$filtered_vcf) {
    exit(3);
  }
  tripal_set_job_progress($job_id, 50);

  // Sort.
  //================
  print "\nSorting the filtered VCF\n";
  $filtered_sorted_vcf = vcf_filter_sort_filtered_file($filtered_vcf, $drush_use_limited);
  if (!$filtered_sorted_vcf) {
    exit(4);
  }

  // Almost there!
  tripal_set_job_progress($job_id, 90);

  // Format.
  //===============
  print "\nMove VCF file $filtered_sorted_vcf to the name we expect: $filepath\n";
  shell_exec("mv $filtered_sorted_vcf $filepath");
  if (!file_exists($filepath)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Unable to move Filtered VCF into expected file director: $filepath");
      exit(5); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
    }else{
      print "ERROR: Unable to move Filtered VCF into expected file director: $filepath\n";
      exit(5);
    }
  }

  // Done!
  tripal_set_job_progress($job_id, 100);
}

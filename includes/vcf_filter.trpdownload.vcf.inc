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
 */
function vcf_filter_vcf_generate_file($variables, $job_id = NULL) {

  // Create the file and ready it for writting to.
  $filepath = variable_get('trpdownload_fullpath', '') . $variables['filename'];
  drush_print("File: " . $filepath);


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

  // Filter.
  //===============
  drush_print("\nFiltering the Original VCF");
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q']);
  if (!$filtered_vcf) {
    exit(3);
  }

  // Almost there!
  tripal_set_job_progress($job_id, 90);

  // Format.
  //===============
  drush_print("\nMove VCF file to the name we expect.");
  shell_exec("mv $filtered_vcf $filepath");
  if (!file_exists($filepath)) {
    drush_set_error("ERROR: Unable to move Filtered VCF into expected file director: $filepath");
    exit(4); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
  }

  // Done!
  tripal_set_job_progress($job_id, 100);
}


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

  // Create the file and ready it for writting to.
  $filepath = variable_get('trpdownload_fullpath', '') . $variables['filename'];
  drush_print("File: " . $filepath);
  $FILE = fopen($filepath, 'w') or die ('Unable to create file to write to');

  // Get the VCF file details.
  $vcf_file = vcf_filter_get_vcf_file($variables['q']['vcf_file_id']);
  drush_print("\nVCF File (".$vcf_file->name."): ".$vcf_file->file_path);

  // Print out parameters.
  drush_print("Parameters:");
  foreach ($variables['q'] as $label => $value) {
    drush_print('  ' . $label . ': ' . $value);
  }

  drush_print("\nFiltering the Original VCF");
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q']);
  $INFILE = fopen($filtered_vcf, 'r') or die ('Unable to read filtered VCF file.');

  drush_print("\nConversion from VCF to A/B Biparental Format.");
  fputs($FILE, implode("\t", array('test','test','test')));
  drush_log('Not yet implemented so simply printed a test line to the file.','warning');

  fclose($FILE);
}


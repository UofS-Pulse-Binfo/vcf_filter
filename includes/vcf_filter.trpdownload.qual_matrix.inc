<?php
/**
 * @file
 * Handles download of VCF files in SNP Matrix Format.
 */

/**
 * Generate SNP Matrix File from VCF.
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
function vcf_filter_qual_matrix_generate_file($variables, $job_id = NULL) {

  // Create the file and ready it for writting to.
  $filepath = variable_get('trpdownload_fullpath', '') . $variables['filename'];
  drush_print("File: " . $filepath);

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

  drush_print("\nConversion from VCF to Quality Matrix Format.");
  $format_tmp = variable_get('file_temporary_path') . '/' . uniqid();
  $command = 'vcftools --vcf ' . escapeshellarg($filtered_vcf) . ' --geno-depth --out ' . escapeshellarg($format_tmp);
  drush_print(shell_exec($command));

  drush_print("\nMove Formatted file to the name we expect.");
  shell_exec("mv $format_tmp.gdepth $filepath");

}


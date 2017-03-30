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

  $files = db_query('SELECT * FROM {vcf_files}')->fetchAllAssoc('vcf_file_id', PDO::FETCH_ASSOC);
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
function vcf_filter_filter_file($file_path, $filter_criteria) {
  $new_file = '';

  drush_log('Not yet implemented.','warning');

  return $new_file;
}

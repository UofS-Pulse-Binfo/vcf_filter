<?php
/**
 * @file
 * API Functions for this module.
 */

/**
 * Retrieve all VCF files.
 */
function vcf_filter_get_vcf_files() {

  $files = db_query('SELECT * FROM {vcf_files}')->fetchAllAssoc('vcf_file_id', PDO::FETCH_ASSOC);
  return $files;
}


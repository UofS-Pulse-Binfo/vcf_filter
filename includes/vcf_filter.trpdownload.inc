<?php
/**
 * @file
 * Contains hooks related to the Tripal Download API.
 */

// Generate File functions have been moved into their own files:
// A/B Biparental Format.
require_once('vcf_filter.trpdownload.abh.inc');
// SNP Matrix.
require_once('vcf_filter.trpdownload.matrix.inc');
// Basic VCF.
require_once('vcf_filter.trpdownload.vcf.inc');

/**
 * Implements hook_register_trpdownload_type().
 */
function vcf_filter_register_trpdownload_type() {
  $types = array();

  // The key is the machine name of my download type.
  $types['vcf_filter_abh'] = array(
    'type_name' => 'VCF Filter',
    'format' => 'A/B Biparental Format',
    'functions' => array(
      'generate_file' => 'vcf_filter_abh_generate_file',
    ),
  );

  $types['vcf_filter_matrix'] = array(
    'type_name' => 'VCF Filter',
    'format' => 'SNP Matrix Format',
    'functions' => array(
      'generate_file' => 'vcf_filter_matrix_generate_file',
    ),
  );

  $types['vcf_filter_VCF'] = array(
    'type_name' => 'VCF Filter',
    'format' => 'VCF Format',
    'functions' => array(
      'generate_file' => 'vcf_filter_vcf_generate_file',
    ),
  );

  return $types;
}

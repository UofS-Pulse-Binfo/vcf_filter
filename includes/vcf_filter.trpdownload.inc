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
// Quality Matrix.
require_once('vcf_filter.trpdownload.qual_matrix.inc');
// Basic VCF.
require_once('vcf_filter.trpdownload.vcf.inc');
// Bgzipped VCF file
require_once('vcf_filter.trpdownload.bgzipped.inc');
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

  $types['vcf_filter_qual_matrix'] = array(
    'type_name' => 'VCF Filter',
    'format' => 'Quality Matrix Format',
    'functions' => array(
      'generate_file' => 'vcf_filter_qual_matrix_generate_file',
    ),
  );

  $types['vcf_filter_VCF'] = array(
    'type_name' => 'VCF Filter',
    'format' => 'VCF Format',
    'functions' => array(
      'generate_file' => 'vcf_filter_vcf_generate_file',
    ),
  );

  $types['vcf_filter_bgzipped'] = array(
    'type_name' => 'VCF Filter',
    'format' => 'Bgzipped VCF Format',
    'functions' => array(
      'generate_file' => 'vcf_filter_bgzipped_generate_file',
      'get_file_suffix' => 'vcf_filter_tar_extension',
    ),
  );

  return $types;
}
/*
function vcf_filter_tar_extension ($vars){
  return 'tar';
}
*/

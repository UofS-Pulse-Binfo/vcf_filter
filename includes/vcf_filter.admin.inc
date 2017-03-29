<?php
/**
 * @file
 * Administration for this module.
 */

/**
 * Provide a landing page for this module by listing VCF files.
 */
function vcf_filter_admin_list() {
  $output = '';

  $output .= '<p>This module provides a form so users can custom filter existing VCF files and export in a variety of formats. In order to make a VCF file available to users, it must first be included below:</p>';

  $output .= l('Add', 'admin/tripal/extension/vcf_filter/add');

  $table = array(
    'header' => array('Human-readable Name', 'File', 'Operations'),
    'rows' => array(),
  );

  // If there are no VCF files yet, then tell the admin this.
  $table['rows'][] = array(array('data' => 'None.', 'colspan' => 3));

  $output .= theme('table', $table);

  return $output;
}

/**
 * Form API Definition for VCF File admin form.
 */
function vcf_filter_admin_vcf_file_form($form, &$form_state) {

}

/**
 * VCF File admin form: Validation.
 */
function vcf_filter_admin_vcf_file_form_validate($form, &$form_state) {

}

/**
 * VCF File admin form: Submission.
 */
function vcf_filter_admin_vcf_file_form_submit($form, &$form_state) {

}


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
  $form_state['rebuild'] = FALSE;
  $form_state['redirect'] = 'admin/tripal/extension/vcf_filter';

  $form['file'] = array(
    '#type' => 'textfield',
    '#title' => 'VCF File (absolute path)',
    '#description' => 'The absolute path to your VCF file. This file must already exist.',
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Human-readable Name',
    '#description' => 'This is the name shown to your users so make sure it is descriptive and uniquely identifies the VCF file',
  );

  $form['num_SNPs'] = array(
    '#type' => 'textfield',
    '#title' => 'Number of SNPs',
    '#description' => 'The number of SNPs in the file.',
  );

  $form['assembly'] = array(
    '#type' => 'textfield',
    '#title' => 'Backbone (e.g. Assembly)',
    '#description' => 'The name of the sequence assembly the SNPs were called on.',
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => 'Description',
    '#description' => 'This should include the method used to generate the file and any filtering that has already been done. It may also be helpful to include some stats about the file such as average read depth to give users some context when filtering.',
  );

  $op = ucwords($form_state['build_info']['args'][0]);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $op
  );

  return $form;
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
  $form_state['rebuild'] = FALSE;
  $form_state['redirect'] = 'admin/tripal/extension/vcf_filter';

}


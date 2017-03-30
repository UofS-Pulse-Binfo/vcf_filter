<?php
/**
 * @file
 * Contains implementation of the user filter form.
 */

/**
 * Form API definition for the VCF Filter form.
 */
function vcf_filter_form($form, &$form_state) {

  $form['intro'] = array(
    '#type' => 'item',
    '#markup' => 'With the increasing size of datasets, it becomes necessary for researchers to take advantage of command-line tools. This form provides an interface to VCF Tools, to facillitate custom filtering of VCF files and exporting of these files in common formats.',
  );

  $form['s1'] = array(
    '#type' => 'fieldset',
    '#title' => 'Choose your VCF File.',
    '#description' => 'The following table contains all the available VCF Files. Choose the one you would like to filter and export by selecting the circle at the beginning of the appropriate row.',
  );

  // Get the list of files keyed by vcf_file_id.
  $files = vcf_filter_get_vcf_files(); 
  $form['files'] = array(
    '#type' => 'hidden',
    '#value' => serialize($files),
    '#serialized' => TRUE,
  );

  // Add number formatting to number of SNPs
  foreach ($files as $vcf_file_id => $d) { $files[$vcf_file_id]['num_snps'] = number_format($d['num_snps']); }
  
  // Create a table of VCF file options.
  $header = array(
    'name' => t('Name'),
    'backbone' => t('Assembly'),
    'num_snps' => t('Number of SNPs'),
  );
  $form['s1']['vcf_file_id'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $files,
    '#empty' => t('No VCF Files available.'),
    '#multiple' => FALSE,
  );

  $form['s2'] = array(
    '#type' => 'fieldset',
    '#title' => 'Specify filter criteria.',
  );

  $form['s3'] = array(
    '#type' => 'fieldset',
    '#title' => 'Pick your Export format.',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Filter',
  );

  return $form;
}

/**
 * VCF Filter Form: Validation.
 */
function vcf_filter_form_validate($form, &$form_state) {

}

/**
 * VCF Filter Form: Submit.
 */
function vcf_filter_form_submit($form, &$form_state) {
  dpm($form_state, 'form state');
}


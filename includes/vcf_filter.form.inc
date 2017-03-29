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

}


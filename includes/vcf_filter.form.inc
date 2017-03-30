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
    '#required' => TRUE,
    '#header' => $header,
    '#options' => $files,
    '#empty' => t('No VCF Files available.'),
    '#multiple' => FALSE,
  );

  $form['s2'] = array(
    '#type' => 'fieldset',
    '#title' => 'Specify filter criteria.',
    '#tree' => TRUE,
  );

  $form['s2']['read_depth'] = array(
    '#type' => 'textfield',
    '#title' => 'Minimum Read Depth',
    '#description' => 'Remove all SNPs with a combined read depth below that specified here.',
    '#default_value' => '5',
  );

  $form['s3'] = array(
    '#type' => 'fieldset',
    '#title' => 'Pick your Export format.',
    '#description' => 'Select one of the formats listed below and the filtered VCF will be converted accordingly. Keep in mind that if you choose a format with no quality information you will have to have done a very goood job of filtering at this point to ensure you are working with good data.'
  );

  $header = array(
    'name' => 'Format',
    'has_quality' => 'Has Quality Info?',
    'description' => 'Description',
  );
  $options = array(
    'abh' => array(
      'name' => 'A/B Format',
      'has_quality' => 'No',
      'description' => 'Alleles are coded as A/B based on the parents. This format is only suitable for biparental crosses',
    ),
    'matrix' => array(
      'name' => 'SNP Matrix',
      'has_quality' => 'No',
      'description' => 'Variant by Germplasm matrix of SNP calls.',
    ),
    'vcf' => array(
      'name' => 'Variant Call Format (VCF)',
      'has_quality' => 'Yes',
      'description' => 'A variant by germplasm matrix with each cell containing a combination of SNP call and quality information. See the '.l('Specification', 'http://samtools.github.io/hts-specs/VCFv4.2.pdf').' for more information.',
    ),
  );
  $form['s3']['format'] = array(
    '#type' => 'tableselect',
    '#required' => TRUE,
    '#header' => $header,
    '#options' => $options,
    '#multiple' => FALSE,
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
  $form_state['rebuild'] = FALSE;

  // Process the paramters into the query string.
  // First grab all the filter criteria.
  $q = $form_state['values']['s2'];
  // Then grab the VCF file to be filtered.
  $q['vcf_file_id'] = $form_state['values']['vcf_file_id'];
  // No need to grab the format b/c that is determined by the tripal download page redirected to.

  // Use format to determine redirect:
  $url = array(
    'abh' => '/filter_vcf/ABH',
    'matrix' => '/filter_vcf/SNP-Matrix',
    'vcf' => '/filter_vcf/VCF'
  );
  $form_state['redirect'] = array(
    $url[ $form_state['values']['format'] ],
    array(
      'query' => $q
    ),
  );
}


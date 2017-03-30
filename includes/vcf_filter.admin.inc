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
    'header' => array('Human-readable Name', 'Backbone', 'File', 'Operations'),
    'rows' => array(),
  );

  $results = db_query('SELECT * FROM {vcf_files}');
  foreach ($results as $r) {
    $table['rows'][] = array(
      'name' => $r->name,
      'backbone' => $r->backbone,
      'file' => $r->file_path,
      l('edit', 'admin/tripal/extension/vcf_filter/edit/'.$r->vcf_file_id) .' | '
        . l('delete', 'admin/tripal/extension/vcf_filter/delete/'.$r->vcf_file_id)
    );
  }
  // If there are no VCF files yet, then tell the admin this.
  if (empty($table['rows'])) {
    $table['rows'][] = array(array('data' => 'None.', 'colspan' => 3));
  }

  $output .= theme('table', $table);

  return $output;
}

/**
 * Form API Definition for VCF File admin form.
 */
function vcf_filter_admin_vcf_file_form($form, &$form_state) {

  $obj = NULL;
  if (isset($form_state['build_info']['args'][1])) {
    $obj = db_query('SELECT * FROM {vcf_files} WHERE vcf_file_id=:id', 
      array(':id' => $form_state['build_info']['args'][1]))->fetchObject();
  }

  $form['vcf_file_id'] = array(
    '#type' => 'hidden',
    '#value' => (isset($obj->vcf_file_id)) ? $obj->vcf_file_id : NULL,
  );

  if ($form_state['build_info']['args'][0] == 'delete') {

    $table = array(
      'header' => NULL,
      'rows' => array(
        array(array('data' => 'VCF File', 'header' => TRUE), $obj->file_path),
        array(array('data' => 'Human-readable Name', 'header' => TRUE), $obj->name),
        array(array('data' => 'Backbone', 'header' => TRUE), $obj->backbone),
        array(array('data' => 'Number of SNPs', 'header' => TRUE), $obj->num_snps),
      ),
    );
    $form['info'] = array(
      '#type' => 'markup',
      '#markup' => '<p>Would you like to delete the following VCF File?</p>'
        .theme('table', $table)
        .'<p><strong>Description:</strong> '.$obj->description.'</p>',
    );

    $form['name'] = array(
      '#type' => 'hidden',
      '#value' => $obj->name,
    );

    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => 'Delete',
      '#suffix' => '<span> ' . l('or Cancel', '/admin/tripal/extension/vcf_filter') . '</span>',
    );

    return $form;
  }
 
  $form['file'] = array(
    '#type' => 'textfield',
    '#title' => 'VCF File (absolute path)',
    '#description' => 'The absolute path to your VCF file. This file must already exist.',
    '#default_value' => (isset($obj->file_path)) ? $obj->file_path : '',
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Human-readable Name',
    '#description' => 'This is the name shown to your users so make sure it is descriptive and uniquely identifies the VCF file',
    '#default_value' => (isset($obj->name)) ? $obj->name : '',
  );

  $form['num_SNPs'] = array(
    '#type' => 'textfield',
    '#title' => 'Number of SNPs',
    '#description' => 'The number of SNPs in the file.',
    '#default_value' => (isset($obj->num_snps)) ? $obj->num_snps : '',
  );

  $form['assembly'] = array(
    '#type' => 'textfield',
    '#title' => 'Backbone (e.g. Assembly)',
    '#description' => 'The name of the sequence assembly the SNPs were called on.',
    '#default_value' => (isset($obj->backbone)) ? $obj->backbone : '',
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => 'Description',
    '#description' => 'This should include the method used to generate the file and any filtering that has already been done. It may also be helpful to include some stats about the file such as average read depth to give users some context when filtering.',
    '#default_value' => (isset($obj->description)) ? $obj->description : '',
  );

  $op = ucwords($form_state['build_info']['args'][0]);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => ($op == 'Edit') ? 'Save Changes' : $op,
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

  if ($form_state['values']['op'] == 'Delete') {
    db_query('DELETE FROM {vcf_files} WHERE vcf_file_id=:id', 
      array(':id' => $form_state['values']['vcf_file_id']));
    drupal_set_message('Successfully deleted <em>'.$form_state['values']['name'].'</em>');
  }
  else {
    $record = array(
      'file_path' => $form_state['values']['file'],
      'name' => $form_state['values']['name'],
      'num_snps' => str_replace(',','', $form_state['values']['num_SNPs']),
      'backbone' => $form_state['values']['assembly'],
      'description' => $form_state['values']['description'],
    );

    if ($form_state['values']['op'] == 'Add') {
      drupal_write_record('vcf_files', $record);
    }
    else {
      $record['vcf_file_id'] = $form_state['values']['vcf_file_id'];
      drupal_write_record('vcf_files', $record, 'vcf_file_id');
    }
  }
}


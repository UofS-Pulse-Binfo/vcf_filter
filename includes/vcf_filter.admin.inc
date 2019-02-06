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
        . l('delete', 'admin/tripal/extension/vcf_filter/delete/'.$r->vcf_file_id) .' | '
        . l('access', 'admin/tripal/extension/vcf_filter/access/'.$r->vcf_file_id)
    );
  }
  // If there are no VCF files yet, then tell the admin this.
  if (empty($table['rows'])) {
    $table['rows'][] = array(array('data' => 'None.', 'colspan' => 4));
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
        .'<p><strong>Description:</strong> '.$obj->description.'</p>'
        .'<p><strong>Germplasm From Header:</strong> '.$obj->specific_germplasm.'</p>'
        .'<p><strong>Chromosome format:</strong> '.$obj->chromosome_format.'</p>',
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
  
  $form['specific_germplasm'] = array(
    '#type' => 'textarea',
    '#title' => 'Germplasm From Header',
    '#description' => 'This should include all names of germplasm (individuals) from the header of VCF file. If this textarea is not filled, shell command will be used to find the header and extract all germplasm names but it may slow down this module a lot.',
    '#default_value' => (isset($obj->specific_germplasm)) ? $obj->specific_germplasm : '',
  );
  
  $form['chromosome_format'] = array(
    '#type' => 'textarea',
    '#title' => 'Chromosome format',
    '#description' => 'This should indicate how chromosome names are formatted in this VCF file. For example, Chr1, chr1, Chr01 or name with a prefix like Lc, Ca, Gm, Pv or Mt.',
    '#default_value' => (isset($obj->chromosome_format)) ? $obj->chromosome_format : '',
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
    db_query('DELETE FROM {vcf_files_perm} WHERE vcf_file_id=:id',
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
      'specific_germplasm' => $form_state['values']['specific_germplasm'],
      'chromosome_format' => $form_state['values']['chromosome_format'],
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

/**
 * Form API Definition for VCF File Permissions form.
 */
function vcf_filter_admin_vcf_file_perm_form($form, &$form_state) {

  $obj = NULL;
  if (isset($form_state['build_info']['args'][0])) {
    $obj = db_query('SELECT * FROM {vcf_files} WHERE vcf_file_id=:id',
      array(':id' => $form_state['build_info']['args'][0]))->fetchObject();
  }

  $form['#attached']['css'][] = array(
    'data' => '
      table caption {
        margin-top: 50px;
        font-weight: bold;
        font-size: 1.2em;
        text-align: left;
      }
      th.name, td.name {
        width: 300px;
      }
      input[type=text] {
        width: 250px;
      }
      td.add {
        background-color: #e1e2dc;
        border-top: 1px solid #bebfb9;
      }
      td.add input[type=submit] {
        margin-top: 10px;
        margin-bottom: 0px;
      }
    ',
    'type' => 'inline',
  );

  $form['vcf_file_id'] = array(
    '#type' => 'hidden',
    '#value' => (isset($obj->vcf_file_id)) ? $obj->vcf_file_id : NULL,
  );

  $form['info'] = array(
    '#type' => 'markup',
    '#theme' => 'table',
    '#header' => NULL,
    '#rows' => array(
      array(array('data' => 'VCF File', 'header' => TRUE), $obj->file_path),
      array(array('data' => 'Human-readable Name', 'header' => TRUE), $obj->name),
      array(array('data' => 'Backbone', 'header' => TRUE), $obj->backbone),
      array(array('data' => 'Number of SNPs', 'header' => TRUE), $obj->num_snps),
      array(array('data' => 'Description', 'header' => TRUE), $obj->description),
      array(array('data' => 'Germplasm From Header', 'header' => TRUE), $obj->specific_germplasm),
      array(array('data' => 'Chromosome format', 'header' => TRUE), $obj->chromosome_format),
    ),
  );

  // ROLES:
  //---------------
  $form['role'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#title' => 'Role Permissions',
    '#caption' => 'All users with the following roles have access to <em>'.$obj->name.'</em>.',
    '#theme' => 'vcf_filter_admin_form_perm_table',
  );

  $count = 0;
  $resource = db_query('
    SELECT r.rid, r.name
    FROM {vcf_files_perm} perm
    LEFT JOIN {role} r ON r.rid=perm.rid
    WHERE perm.rid IS NOT NULL AND vcf_file_id=:id',
    array(':id' => $obj->vcf_file_id));
  foreach ($resource as $r) {
    $count++;

    $form['role'][$r->rid]['id'] = array(
      '#type' => 'item',
      '#value' => $r->rid,
    );

    $form['role'][$r->rid]['name'] = array(
      '#type' => 'item',
      '#value' => $r->name,
    );

    $form['role'][$r->rid]['remove_url'] = array(
      '#type' => 'item',
      '#value' => 'admin/tripal/extension/vcf_filter/access/'.$obj->vcf_file_id.'/remove-role/'.$r->rid,
    );

  }
  if (!$count) {
    $form['role']['none']['name'] = array(
      '#type' => 'item',
      '#value' => 'None.',
    );
  }

  $options = array();
  $resource = db_query('SELECT rid, name FROM {role}');
  foreach ($resource as $r) {
    $options[ $r->rid ] = $r->name;
  }
  $form['role']['add']['form_element'] = array(
    '#type' => 'select',
    '#title' => 'Role to give permission to',
    '#options' => $options,
    '#empty_value' => 'None',
  );

  $form['role']['add']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add Role',
  );

  // USER:
  //---------------
  $form['user'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#title' => 'User Permissions',
    '#caption' => 'The following users have access to <em>'.$obj->name.'</em>.',
    '#theme' => 'vcf_filter_admin_form_perm_table',
  );

  $count = 0;
  $resource = db_query('
    SELECT u.uid, u.name
    FROM {vcf_files_perm} perm
    LEFT JOIN {users} u ON u.uid=perm.uid
    WHERE perm.uid IS NOT NULL AND vcf_file_id=:id',
    array(':id' => $obj->vcf_file_id));
  foreach ($resource as $r) {
    $count++;

    $form['user'][$r->uid]['id'] = array(
      '#type' => 'item',
      '#value' => $r->uid,
    );

    $form['user'][$r->uid]['name'] = array(
      '#type' => 'item',
      '#value' => $r->name,
    );

    $form['user'][$r->uid]['remove_url'] = array(
      '#type' => 'item',
      '#value' => 'admin/tripal/extension/vcf_filter/access/'.$obj->vcf_file_id.'/remove-user/'.$r->uid,
    );

  }
  if (!$count) {
    $form['user']['none']['name'] = array(
      '#type' => 'item',
      '#value' => 'None.',
    );
  }

  $form['user']['add']['form_element'] = array(
    '#type' => 'textfield',
    '#title' => 'User to give permission to',
    '#autocomplete_path' => 'autocomplete/vcf_filter/user',
  );

  $form['user']['add']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add User',
  );

  return $form;
}

/**
 * VCF File Permissions form: Validation.
 */
function vcf_filter_admin_vcf_file_perm_form_validate($form, &$form_state) {

}

/**
 * VCF File Permissions form: Submission.
 */
function vcf_filter_admin_vcf_file_perm_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == 'Add Role') {
    $record = array(
      'vcf_file_id' => $form_state['values']['vcf_file_id'],
      'rid' => $form_state['values']['role']['add']['form_element'],
    );
    drupal_write_record('vcf_files_perm', $record);
  }
  elseif ($form_state['clicked_button']['#value'] == 'Add User') {
    $uid = db_query('SELECT uid FROM {users} WHERE name=:name',
      array(':name' => $form_state['values']['user']['add']['form_element']))->fetchField();
    if ($uid) {
      $record = array(
        'vcf_file_id' => $form_state['values']['vcf_file_id'],
        'uid' => $uid,
      );
      drupal_write_record('vcf_files_perm', $record);
    }
  }
}

/**
 * Callback to remove permissions (both user or role).
 *
 * NOTE: There is no display or confirmation for removing users/roles. This callback will
 * redirect to the access page when done.
 *
 * @param $vcf_file_id
 *   The pkey of the VCF file we are removing permissions from.
 * @param $action
 *   Either 'remove-role' OR 'remove-user' based on which type of permission we need to remove.
 * @param $id
 *   The pkey of the role or user to remove permission to. If $action is remove-role then
 *   this is the rid; if it's remove-user then this is the uid.
 */
function vcf_filter_admin_vcf_file_perm_remove_callback($vcf_file_id, $action, $id) {

  if ($action == 'remove-role') {

    $num_deleted = db_delete('vcf_files_perm')
      ->condition('vcf_file_id', $vcf_file_id)
      ->condition('rid', $id)
      ->execute();
    if ($num_deleted) {
      drupal_set_message('Successfully removed permission to role.');
    }

  }
  elseif ($action == 'remove-user') {

    $num_deleted = db_delete('vcf_files_perm')
      ->condition('vcf_file_id', $vcf_file_id)
      ->condition('uid', $id)
      ->execute();
    if ($num_deleted) {
      drupal_set_message('Successfully removed permission to user.');
    }

  }

  // Redirect to access page.
  drupal_goto('admin/tripal/extension/vcf_filter/access/'.$vcf_file_id);
}

/**
 * Theme both permissions tables.
 */
function vcf_filter_admin_form_perm_table($vars) {
  $element = $vars['element'];
  $output = '';

  $table = array(
    'caption' => $element['#title'].': '.$element['#caption'],
    'header' => array('Name','Operations'),
    'rows' => array(),
  );

  foreach (element_children($element) as $key) {

    if ($key == 'add') {
      $table['rows'][] = array(
        array('data' => render($element[$key]['form_element']), 'class' => array('name', 'add')),
        array('data' => render($element[$key]['submit']), 'class' => array('op', 'add')),
      );
    }
    else {
      $remove_link = '';
      if (isset($element[$key]['remove_url'])) {
        $remove_link = l('remove', $element[$key]['remove_url']['#value']);
      }

      $table['rows'][] = array(
        array('data' => $element[$key]['name']['#value'], 'class' => array('name')),
        array('data' => $remove_link, 'class' => array('op')),
      );
    }

  }

  return theme('table', $table);
}

/**
 * Autocomplete: Usernames in VCF Access permissions.
 */
function vcf_filter_user_autocomplete($string = '') {
  $matches = array();
  if ($string) {
    $result = db_query('SELECT name FROM {users} WHERE LOWER(name) ~ LOWER(:string)', array(':string' => $string));
    foreach ($result as $user) {
      $matches[$user->name] = check_plain($user->name);
    }
  }

  drupal_json_output($matches);
}

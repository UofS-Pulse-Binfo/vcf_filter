<?php
/**
 * @file
 * Contains implementation of the user filter form.
 */

/**
 * Form API definition for the VCF Filter form.
 */
function vcf_filter_position_search_form($form, &$form_state) {

  dpm($form_state, '$form_state');

  $form['#attached']['css'][] = array(
    'data' => '
      .position_search_option {
        font-weight: bold;
        margin-top: 5px;
        padding-top: 15px;
      }
    ',
    'type' => 'inline',
  );

  $form['intro'] = array(
    '#type' => 'item',
    '#markup' => 'VCF Position Search is part of modeule VCF Bulk Export and it supplies function of searching marker/site cross all of our VCF files.',
  );

  $files = vcf_filter_get_vcf_files();

  $form['s1'] = array(
    '#type' => 'fieldset',
    '#title' => 'Search Criteria',
    '#description' => 'Please fill in search criteria. You search by marker name or backbone name plus postion.',

  );
  $form['s1']['marker_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Marker Name',
    '#attributes' =>array('placeholder' => 'Chr1p123456789'),
    '#prefix' => '<div class="position_search_option">',
    '#suffix' => '</div>',
  );

  $form['s1']['or'] = array(
    '#type' => 'markup',
    '#markup' => '<div style="color: #314355; font-weight: bold">OR</div>',
  );

  $form['s1']['backbone'] = array(
    '#type' => 'textfield',
    '#title' => 'Backbone Name',
    '#attributes' =>array('placeholder' => 'Chr1'),
    '#prefix' => '<div class="position_search_option">',
  );
  $form['s1']['position'] = array(
    '#type' => 'textfield',
    '#title' => 'Position',
    '#attributes' =>array('placeholder' => '123456789'),
    '#suffix' => '</div>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Search',
  );

  /*
  if form state values set...
    query for results.

    display results using form #type markup. use theme_table to create a table.
  */
  if (isset($form_state['values'])){
    //$marker_4vcftools = $form_state['values']['marker_name'] . ':' . $form_state['values']['position'] . '..' . $form_state['values']['position'];
    $current_url = url('filter_vcf', array('absolute' => TRUE));

    if (!empty($form_state['values']['marker_name'])){
      $marker_4search = trim($form_state['values']['marker_name']);
      $marker_4search_exp = explode('p', $marker_4search);
      $marker_4search = $marker_4search_exp[0] . ':' . $marker_4search_exp[1];
      $available_files = position_check_use_bcftools($marker_4search, $files);
      $marker_4search = $marker_4search . '..' . $marker_4search_exp[1];
    }
    else{
      $marker_4search_bb = trim($form_state['values']['backbone']);
      $marker_4search_p = trim($form_state['values']['position']);
      $marker_4search = $marker_4search_bb . ':' . $marker_4search_p;
      $available_files = position_check_use_bcftools($marker_4search, $files);
      $marker_4search = $marker_4search . '..' . $marker_4search_p;
    }
    dpm($available_files, '$available_files');
    //$available_files = position_check_use_bcftools($marker_4vcftools, $files);
    //@test list all files
    //$available_files = $files;
    if (!empty($available_files)){
	    // list all files and provide url if there is at least one file available
	    // list file name also link to vcf_file page by url
	    $item_files = array(
	    	'items' => array(),
		    'title' => '',
		    'type' => 'ul',
		    'attributes' => array(),
	    );

      foreach($available_files as $one_vcf_id => $one_vcf_info ){
        //$list .= theme_item_list('items'=> $one_vcf_info['name'], 'title' => NULL, 'type' => 'ul', 'attributes' => );
        $item_files['items'][$one_vcf_id]['data'] = l($one_vcf_info['name'],
          $current_url,
          array('query' => array(
            'vcf_file_id' => $one_vcf_id,
            'one_region'=> $marker_4search
            )
          )
        );
      }
	    $list = theme_item_list( $item_files);
      $form['s2'] = array(
        '#type' => 'fieldset',
        '#title' => 'Search Results',
        '#description' => 'List of VCF files which include this postion.',
      );
      $form['s2']['list'] = array(
        '#type' => 'markup',
        '#markup' => $list,
      );

    }
    else{
      // no file available, print warning messages
      $form['s2'] = array(
        '#type' => 'fieldset',
        '#title' => 'Search Results',
        '#description' => '',
      );
      $form['s2']['list'] = array(
        '#type' => 'markup',
        '#markup' => 'No serach result. This marker/site can not be found in any file.',
      );
    }
  }

  return $form;
}

/**
 * VCF Filter Form: Validation.
 */
function vcf_filter_position_search_form_validate($form, &$form_state) {

}

/**
 * VCF Filter Form: Submit.
 */
function vcf_filter_position_search_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;

}


/**
 * use bcftools to check if one postion(marker) exist in
 * bcftools command: bcftools view --no-header -r Chr:position vcf_name.vcf.gz
 * note: vcf file must be compressed and indexed, commands:
 *  bgzip -c file_name.vcf > file_name.vcf.gz
 *  tabix -p file_name.vcf.gz
 */
function position_check_use_bcftools($position, $all_vcf_files){
    $vcf_file_w_marker = $all_vcf_files;
  foreach($all_vcf_files as $file_id => $file_info){
    $command = 'bcftools view --no-header -r ';
    $command .= $position . ' ' . $file_info['file_path'] . '.gz';
    $output = shell_exec($command);
    if (empty($output)){
      unset($vcf_file_w_marker[$file_id]);
    }

  }
  return $vcf_file_w_marker;
}


/**
  * Grap data in a file to use in Specifiy_fileter_criterria:Germplasm
  * provide a exampel file from directory: example_data
*/
function user_example_germplasm_names_file(){
  $file = drupal_get_path('module', 'vcf_filter') . '/example_data/Example_Germplasm_names.csv';

  return file_get_contents(drupal_realpath($file));
}

/**
  * Grap data in a file to use in Specifiy_fileter_criterria:Germplasm
  * grep germplasm names from vcf file
  * steps:
  *   1. use command to obtain header line, convert from string to array
  *   2. remove first 9 elements, and keep all germplasm names
  *   3. convert from array to string and return
*/
function user_chart_germplasm_names_file($one_file_id){
  $vcf_file = vcf_filter_get_vcf_file($one_file_id);
  $command_get_germplasm = 'grep "#CHROM" ' . $vcf_file->file_path;
  $header = shell_exec($command_get_germplasm);
  $temp_file = variable_get('file_temporary_path') . '/' .  'germplasm_name_all'. uniqid();
  $header = explode("\t", $header);
  for ($i=0;$i<9;$i++){
    array_shift($header);

  }
  $header = implode("\n", $header);
  return $header;
}

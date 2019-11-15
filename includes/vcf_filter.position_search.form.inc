<?php
/**
 * @file
 * Contains implementation of the user search form.
 */

/**
 * Form API definition for the position search form.
 */
function vcf_filter_position_search_form($form, &$form_state) {

  //@test dpm($form_state, '$form_state');
  $form['#attached']['css'][] = array(
    'data' => '
      .position_search_option {
        font-weight: bold;
        margin-top: 5px;
        padding-top: 15px;
      }
      .description {
        font-style: italic;
        font-weight: normal;
        font-size: 4;
      }
    ',
    'type' => 'inline',
  );

  $form['intro'] = array(
    '#type' => 'item',
    '#markup' => 'VCF Position Search is part of modeule VCF Bulk Export and it supplies function of searching variant cross all of our VCF files.',
  );

  $files = vcf_filter_get_vcf_files();

  $form['s1'] = array(
    '#type' => 'fieldset',
    '#title' => 'Search Criteria',
    '#description' => 'Please fill in search criteria.',

  );
  $form['s1']['marker_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Variant Name',
    '#attributes' =>array('placeholder' => 'Chr1p123456789'),
    '#description' => '<span class="description">The name of the variant (e.g. LcChr6p161100389, Lcu.2RBY.Chr6p407560335)</span>',
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
    '#description' => '<span class="description">The backbone of the variant (e.g. LcChr6, Lcu.2RBY.Chr6)</span>',
    '#attributes' =>array('placeholder' => 'Chr1'),
    '#prefix' => '<div class="position_search_option">',
  );
  $form['s1']['position'] = array(
    '#type' => 'textfield',
    '#title' => 'Position',
    '#description' => '<span class="description">The position of the variant (e.g. 161100389, 407560335)</span>',
    '#attributes' =>array('placeholder' => '123456789'),
    '#suffix' => '</div>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Search',
  );

  /*
    update form if search criteria is provided by user
    display results using form #type markup. use theme_item_list to provide list of files link to VCF Bulk Export.
  */

  if (isset($form_state['values'])){

    $current_url = url('filter_vcf', array('absolute' => TRUE));
    //two cases: search by marker name or search by backbone+postion
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
    //@test dpm($available_files, '$available_files');

    if (!empty($available_files)){
	    // list all files and provide url if there is at least one file available
	    // list file name also link to vcf_file page by url
      dpm($available_files, '$available_files');
	    $item_files = array(
	    	'items' => array(),
		    'title' => '',
		    'type' => 'ul',
		    'attributes' => array(),
	    );

      foreach($available_files as $one_vcf_id => $one_vcf_info ){
        $item_files['items'][$one_vcf_id]['data'] = l($one_vcf_info['name'].'--Assumbly:'.$one_vcf_info['name'],
          $current_url,
          array('query' => array(
            'vcf_file_id' => $one_vcf_id,
            'one_region'=> $marker_4search
            )
          )
        );
        $item_files('attributes') = 'Assumbly:'.$one_vcf_info['name'];
      }
/*
      $searched_vcf_file_table = array(
        'caption' => 'List of Files:',
        'header' => array('File Name', 'Assembly'),
        'rows' => array(),
      );

      foreach($available_files as $one_vcf_id => $one_vcf_info ){
        array_push($searched_vcf_file_table['rows'], array(l($one_vcf_info['name'], $current_url,
          array('query' => array(
            'vcf_file_id' => $one_vcf_id,
            'one_region'=> $marker_4search
            )
          )
         ), $one_vcf_info['name']) );
      }
      dpm($searched_vcf_file_table, '$searched_vcf_file_table');
*/
	    $list = theme_item_list($item_files);
      //$list = theme('table', $searched_vcf_file_table);
      $form['s2'] = array(
        '#type' => 'fieldset',
        '#title' => 'Search Results',
        '#description' => 'List of VCF files which include this variant.<br>Each file name is linked to VCF Bulk Export with file selected and position filled.',
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
        '#markup' => 'No matched variant in VCF files.',
      );
    }
  }

  return $form;
}

/**
 * VCF Filter Form: Validation.
 */
function vcf_filter_position_search_form_validate($form, &$form_state) {

  // CHECK: Marker name or backbone+position
  if (empty($form_state['values']['marker_name'])) {

    if (empty($form_state['values']['backbone']) && empty($form_state['values']['position'])){
      form_set_error('S1', 'Please provide search criteria.');
    }
    elseif(empty($form_state['values']['backbone'])){
      form_set_error('backbone', 'Backbone is required if search by backbone and position is expected.');
    }
    elseif(empty($form_state['values']['position'])){
      form_set_error('position', 'Position is required if search by backbone and position is expected.');
    }
  }
  elseif(!empty($form_state['values']['backbone']) && !empty($form_state['values']['position'])){
    $bb_and_ps = $form_state['values']['backbone'] . 'p' . $form_state['values']['position'];
    $bb_and_ps = trim($bb_and_ps);
    $form_state['values']['marker_name'] = trim($form_state['values']['marker_name']);
    if ($bb_and_ps != $form_state['values']['marker_name']){
      form_set_error('position', 'Both Variant Name and Backbone/Position are provided but not match, please specifiy only one.');
    }
  }

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

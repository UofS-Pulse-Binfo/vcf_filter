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
      .form-type-checkbox label {
        font-weight: bold;
        position: relative;
        top: 2px;
      }
      table caption {
        font-weight: bold;
        margin-top: 5px;
        padding-top: 15px;
        border-top: 1px solid #d0d0d0;
      }
      .citation {
        font-size: 0.8em;
        font-style: italic;
      }
    ',
    'type' => 'inline',
  );

  $form['intro'] = array(
    '#type' => 'item',
    '#markup' => 'This form provides filtering of existing VCF files and export into common formats. Most of the filter criteria and many of the formats are provided by <a href="https://vcftools.github.io" target="_blank">VCFtools</a><sup>+</sup>.',
  );

  $form['s1'] = array(
    '#type' => 'fieldset',
    '#title' => 'Search Criteria',
    '#description' => 'The following table contains all the available VCF Files. Choose the one you would like to filter and export by selecting the circle at the beginning of the appropriate row.',
    //'#prefix' => '<div id="all-vcf-options">',
    //'#suffix' => '</div>',
  );
  $form['s1']['marker_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Marker Name',
    '#default_value' => '',
  );
  $form['s1']['Backbone'] = array(
    '#type' => 'textfield',
    '#title' => 'Backbone Name',
    '#default_value' => '',
  );
  $form['s1']['Position'] = array(
    '#type' => 'textfield',
    '#title' => 'Position',
    '#default_value' => '',
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
  if ($form_state['values']){

    $files = vcf_filter_get_vcf_files();

    $available_files = position_check_use_bcftools($position, $files);
    dpm($file, '$file');
    /*
  $list_of_files = array();

  $form['s2'] = array(
    '#type' => 'markup',
    '#title' => 'Search Results',
    '#markup' => $list_of_files,
    '#description' => 'All files containing this site.',
  );
  */

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
function position_check_use_bcftools($position, $filepath){



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

<?php
/**
 * @file
 * Contains all field specific code outside the classes.
 */

/**
 * Implements hook_bundle_fields_info().
 *
 * This hook tells Drupal/Tripal about your new field type. Make sure you've created the
 * field (handles basic storage of your data), widget (provides user UI to set data),
 * and formatter (describes display of data on Entity Page) classes. These should be
 * located in the following directory: [your module]/includes/TripalFields/[classname].inc
 * with one file per class. Your field name should be [cv name]__[cvterm name] and the
 * classes should be named [field name], [field_name]_widget, [field name]_formatter
 * for the field, widget and formatter respectively. MAKE SURE YOU'VE CLEARED THE CACHE
 * SINCE ADDING THESE FILES so Tripal magic can find them or the following will fail.
 *
 * @param $entity_type
 *   This should be 'TripalEntity' for all Tripal Content.
 * @param $bundle
 *   This object describes the Type of Tripal Entity (e.g. Organism or Gene) this hook is
 *   being called for. However, since this hook creates field types (by definition not
 *   tied to a specific Tripal Content Type (bundle)) and since a field type will only be
 *   created if it doesn't already exist, this parameter doesn't actually matter.
 *   NOTE: If you do need to determine the bundle in this hook, we suggest inspecting
 *   the data_table since the label can be changed by site administrators.
 *
 * @return
 *   An array of field definitions. Each field in this array will be created if it
 *   doesn't already exist. To trigger create of fields when developing call
 *   tripal_refresh_bundle_fields() for the specific bundle.
 */
function vcf_filter_bundle_fields_info($entity_type, $bundle) {
  $fields = array();

  // Because we are expecting data housed in Chado we can use the 'data_table'
  // property of the bundle to determine if this bundle is really the one
  // we want the field to be associated with.
  if (isset($bundle->data_table) AND ($bundle->data_table == 'stock')) {
    // First add my term.
    tripal_insert_cvterm(array(
      'id' => 'local:stock_quick_filter',
      'name' => 'stock_quick_filter',
      'cv_name' => 'local',
      'definition' => 'Quick filter using VCF Bulk Loader.',
    ));

    // Then describe the field defined by that term.
    $field_name = 'local__stock_quick_filter';
    $field_type = 'local__stock_quick_filter';
    $fields[$field_name] = array(
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => array(
        'type' => 'field_chado_storage',
      ),
    );
  }

  return $fields;
}

/**
 * Implements hook_bundle_instances_info().
 *
 * This hook tells Drupal/Tripal to create a field instance of a given field type on a
 * specific Tripal Content type (otherwise known as the bundle). Make sure to implement
 * hook_bundle_create_fields() to create your field type before trying to create an
 * instance of that field.
 *
 * @param $entity_type
 *   This should be 'TripalEntity' for all Tripal Content.
 * @param $bundle
 *   This object describes the Type of Tripal Entity (e.g. Organism or Gene) the field
 *   instances are being created for. Thus this hook is called once per Tripal Content Type on your
 *   site. The name of the bundle is the machine name of the type (e.g. bio_data_1) and
 *   the label of the bundle (e.g. Organism) is what you see in the interface. Since the
 *   label can be changed by site admin, we suggest checking the data_table to determine
 *   if this is the entity you want to add field instances to.
 * @return
 *   An array of field instance definitions. This is where you can define the defaults
 *   for any settings you use in your field. Each entry in this array will be used to
 *   create an instance of an already existing field.
 */
function vcf_filter_bundle_instances_info($entity_type, $bundle) {
  $instances = array();

  if (isset($bundle->data_table) AND ($bundle->data_table == 'stock')) {
    $field_name = 'local__stock_quick_filter';
    $field_type = 'local__stock_quick_filter';
    $instances[$field_name] =  array(
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Germplasm VCF Quick Filter',
      'description' => 'Quick filter using VCF Bulk Loader.',
      'required' => FALSE,
      'settings' => array(
        'term_vocabulary' => 'local',
        'term_name' => 'stock_quick_filter',
        'term_accession' => 'stock_quick_filter',
        'auto_attach' => FALSE,
        'chado_table' => $bundle->data_table,
        'chado_column' => 'organism_id',
        'base_table' => $bundle->data_table,
      ),
      'widget' => array(
        'type' => 'local__stock_quick_filter_widget',
        'settings' => array(),
      ),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'local__stock_quick_filter_formatter',
          'settings' => array(),
        ),
      ),
    );

  }

  return $instances;
}

/**
 * AJAX-enabled form for [field local__stock_quick_filter_formatter].
 * $select should include:
 *  1. schema:url
 *  2. stock(germplasm) name
 *  3. an array including availalbe(include this stock + user can access) vcf files from vcf_filter 
 */
function tripal_stock_quick_filter_selector_form($form, &$form_state, $select){

  $select_vcf = $select['related_vcf_file'];
  $selected_array_key = $select_vcf[0]['id'];
  $availalbe_file_names = array();
  foreach($select_vcf as $key => $value){
    array_push($availalbe_file_names, $value['name']);
  }
  
  // add css file, so link to vcf_filter can be a button
  $module_path = drupal_get_path('module', 'vcf_filter');
  $form['#attached']['css'][] = $module_path . '/includes/TripalFields/local__stock_quick_filter/theme/local__stock_quick_filter.css';
  
  $form['selecte_a_vcf_file'] = [
    '#type' => 'fieldset',
    '#title' => 'Specify a VCF file for filtering',
    '#tree' => TRUE,
    '#prefix' => '<div id="select_one_vcf_file">',
    '#suffix' => '</div>',
  ];
  
  $form['selecte_a_vcf_file']['vcf_4_select'] = [
    '#type' => 'select',
    '#options'  => $availalbe_file_names,
    '#ajax' => array(
      'callback' => 'ajax_vcf_filter_url_to_one_file_callback',
      'wrapper' => 'select_one_vcf_file',
      'method' => 'replace',
      'effect' => 'fade',
    )
  ];
  
  $form['selecte_a_vcf_file']['vcf_filter_link'] = [
    '#type' => 'markup',
    '#prefix' => '<div class="vf-button">',
    '#markup' => l(
      t('Filter'),
      $select['schema:url'],
      array('query' => array(
        'one_germplasm' => $select['stock_name'],
        'vcf_file_id' => $selected_array_key
        )
      )
    ),
    '#suffix' => '</div>'
  ];

  // if a file is slected, update vcf_file_id of url to vcf_filter
  if (isset($form_state['values']['selecte_a_vcf_file'])) {
    $selected_array_key = $select_vcf[ $form_state['values']['selecte_a_vcf_file']['vcf_4_select'] ]['id'];

    $form['selecte_a_vcf_file']['vcf_filter_link']['#markup'] = l(
      t('Filter'),
      $select['schema:url'],
      array('query' => array(
        'one_germplasm' => $select['stock_name'],
        'vcf_file_id' => $selected_array_key
        )
      )
    );
  }
  return $form;
}
 
 
/**
 * The callback will return the part of the form you want to re-draw.
 */
 
function ajax_vcf_filter_url_to_one_file_callback($form, &$form_state) {

  return $form['selecte_a_vcf_file'];
}

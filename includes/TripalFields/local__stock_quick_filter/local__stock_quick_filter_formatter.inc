<?php
/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__stock_quick_filter_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Germplasm VCF Quick Filter';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__stock_quick_filter');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

   /**
    * @see ChadoFieldFormatter::settingsForm()
    *
    **/

  public function settingsForm($view_mode, $form, &$form_state) {

  }

    /**
    * @see ChadoFieldFormatter::View()
    *
    **/

  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];
    if (isset($items['0']['value']['related_vcf_file'])) {

      $form = drupal_get_form('tripal_stock_quick_filter_selector_form', $items['0']['value']);

      $content = drupal_render($form);

      $element[] = [
        '#type' => 'markup',
        '#markup' => $content,
      ];
    }
    return $element;
  }

    /**
    * @see ChadoFieldFormatter::settingsSummary()
    *
    **/

  public function settingsSummary($view_mode) {
    return '';
  }

}

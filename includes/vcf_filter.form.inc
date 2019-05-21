<?php
/**
 * @file
 * Contains implementation of the user filter form.
 */

/**
 * Form API definition for the VCF Filter form.
 */
function vcf_filter_form($form, &$form_state) {

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
    '#title' => 'Choose your VCF File.',
    '#description' => 'The following table contains all the available VCF Files. Choose the one you would like to filter and export by selecting the circle at the beginning of the appropriate row.',
    '#prefix' => '<div id="all-vcf-options">'
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
    '#ajax' => array(
      'callback' => 'ajax_vcf_filter_show_description_callback',
      'wrapper' => 'all-vcf-options',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  if (isset($_GET['vcf_file_id']) AND is_numeric($_GET['vcf_file_id'])) {
    $form['s1']['vcf_file_id']['#default_value'] = $_GET['vcf_file_id'];
  }
  if (isset($form_state['values']['vcf_file_id'])) {
    $form['s1']['vcf_file_id']['#default_value'] = $form_state['values']['vcf_file_id'];
  }

  $descrip_markup = '';
  if (!empty($form['s1']['vcf_file_id']['#default_value'])) {
    $curr_file = $files[  $form['s1']['vcf_file_id']['#default_value']  ];
    if (!empty($curr_file['description'])) {
      $descrip_markup = '<br /><p><strong>More information on <em>'.$curr_file['name'].'</em>:</strong> ' . $curr_file['description'] . '</p>';
    }
  }
  $form['s1']['file_description'] = array(
    '#type' => 'markup',
    '#markup' => $descrip_markup,
  );

  $form['s2'] = array(
    '#type' => 'fieldset',
    '#title' => 'Specify filter criteria.',
    '#tree' => TRUE,
    '#suffix' => '</div>',
  );

  $form['s2']['restrict_dataset'] = array(
    '#type' => 'fieldset',
    '#title' => 'Restrict dataset to specific germplasm or regions',
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => 'Select a VCF file from above to see specific germplasm.'
  );

    $form['s2']['restrict_dataset']['regions'] = array(
    '#type' => 'textarea',
    '#title' => 'Regions',
    '#description' => 'Only include sites in specific regions. For example, if you want to include all genotypic data for a QTL on Chr1 from 5555 to 6666 then you would enter Chr1:5555..6666 <br>Region input format: Chrom:position-lower-bound..position-upper-bound (e.g., Chr1:111100..222200). ',
    '#attributes' =>array('placeholder' => 'Chr1:111100..222200'),
    '#rows' => 2,
  );

  $form['s2']['restrict_dataset']['germplasm_available'] = array(
    '#type' => 'textarea',
    '#title' => 'Germplasm from selected file',
    '#disable' => TRUE,
    '#description' => 'All germplasm (individuals) in selected file. <strong>Please copy the germplasm that need to be kept to textarea under.</strong>',
    '#rows' => 2,
    );

  if (!empty($form['s1']['vcf_file_id']['#default_value'])) {
    $curr_file = $files[  $form['s1']['vcf_file_id']['#default_value']  ];
    if (!empty($curr_file['specific_germplasm'])) {
      $value_4_germplasm_availalbe = $curr_file['specific_germplasm'];
    }
    if (!empty($curr_file['chromosome_format'])) {
      $value_4_attributes_placeholder = trim($curr_file['chromosome_format'] . ':111111..222222');
      $value_4_attributes_placeholder = array('placeholder' => $value_4_attributes_placeholder);
    }

  }

    $form['s2']['restrict_dataset']['help'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="messages warning">You must <strong>copy</strong> the germplasm you want to keep into the <strong>Keep these Germplasm</strong> or your file will contain all germplasm.</div>'
  );

  $form['s2']['restrict_dataset']['germplasm_select'] = array(
    '#type' => 'textarea',
    '#title' => 'Keep these Germplasm',
    '#description' => 'Only include these germplasm (individuals) in export file. Each germplasm name should be on it\'s own line and must match exactly the names in the file chosen (names are shown above).',
    '#rows' => 2,
    );

  //update $form['s2']['restrict_dataset']
  if ((isset($form_state['values']['vcf_file_id'])) or (isset($_GET['vcf_file_id']) AND is_numeric($_GET['vcf_file_id']) )) {
    $form['s2']['restrict_dataset']['#collapsed'] = FALSE;
    $form['s2']['restrict_dataset']['germplasm_available']['#rows'] = 5;
    $form['s2']['restrict_dataset']['germplasm_available']['#disabled'] = FALSE;
    $form['s2']['restrict_dataset']['germplasm_select']['#rows'] = 5;

    if ($value_4_attributes_placeholder){
      $form['s2']['restrict_dataset']['regions']['#attributes'] = $value_4_attributes_placeholder;
      $form['s2']['restrict_dataset']['regions']['#description'] = 'Only include sites in specific regions. For example, if you want to include all genotypic data for a QTL on Chr1 from 5555 to 6666 then you would enter Chr1:5555..6666 <br>Region input format: Chrom:position-lower-bound..position-upper-bound (e.g.,' . $value_4_attributes_placeholder['placeholder'] . ').';
    }

    if ($value_4_germplasm_availalbe){
      $form['s2']['restrict_dataset']['germplasm_available']['#value'] = $value_4_germplasm_availalbe;
    }
    else{
      $form['s2']['restrict_dataset']['germplasm_available']['#value'] = trim(user_chart_germplasm_names_file($form_state['values']['vcf_file_id']));
    }
  }

  if (isset($_GET['one_germplasm'])) {
    $form['s2']['restrict_dataset']['germplasm_select']['#value'] = $_GET['one_germplasm'];
    $form['s2']['restrict_dataset']['#collapsed'] = FALSE;
    $form['s2']['restrict_dataset']['germplasm_select']['#rows'] = 2;
  }

  $form['s2']['bi-allelic'] = array(
    '#type' => 'checkbox',
    '#title' => 'Only include Bi-allelic SNPs',
    '#description' => 'If you check this checkbox, only SNPs with 2 alleles across all individuals will be kept. For example, in the example data below, SNP Chr3p34567 would be removed.',
  );

  $form['s2']['call_read_depth'] = array(
    '#type' => 'textfield',
    '#title' => 'Minimum SNP Call Read Depth',
    '#description' => 'Only include SNP calls that have at least the specified number of reads to support the call. For example, if you specify 5 for this filter then for SNP Chr2p25678 in the example table below, only the call for Germplasm4 will be set to missing data.',
    '#default_value' => '',
  );

  $form['s2']['minor_allele_freq'] = array(
    '#type' => 'textfield',
    '#title' => 'Minor Allele Frequency',
    '#description' => 'Only include SNP positions with a minor allele frequency greater than or equal to this value. Allele frequency is defined as the number of times an allele appears over all individuals at that site, divided by the total number of non-missing alleles at that site. For example, if your enter 45% in this filter then SNPs with a minor allele frequency lower than 45% could be removed (SNP Chr1p12344 in the example data below).',
    '#default_value' => '',
  );

  $form['s2']['max_missing_count'] = array(
    '#type' => 'textfield',
    '#title' => 'Maximum Missing Count',
    '#description' => 'Exclude SNPs with more than this number of missing genotypes over all individuals/germplasm. For example, if you enter 1 for this filter then SNPs with more than 1 missing genotype would be removed (SNP Chr4p48765 in the example data below).',
    '#default_value' => '',
  );

  $form['s2']['max_missing_freq'] = array(
    '#type' => 'textfield',
    '#title' => 'Maximum Missing Frequency',
    '#description' => 'Exclude SNPs based on the proportion of missing data. For example, if you enter 25% for this filter then SNPs with a missing data frequency higher than 25% would be removed (SNP Chr4p48765 in the example data below).',
    '#default_value' => '',
  );

  $example_table = array(
    'caption' => 'Example Table: Example Data for Filter Explanation.',
    'header' => array('SNP Name', 'SNP Backbone', 'SNP Position', 'Germ1', 'Germ2', 'Germ3', 'Germ4', 'Germ5', 'Germ6'),
    'rows' => array(
      array('Chr1p12344', 'Chr1', '12344', 'AA:5', 'TT:12', '', 'TT:15', 'AT:19', 'TT:15'),
      array('Chr2p25678', 'Chr2', '25678', 'GG:7', 'GG:13', 'GG:5', 'TT:2', 'GG:22', 'GT:24'),
      array('Chr3p34567', 'Chr3', '34567', 'AA:5', 'CC:12', 'AC:7', 'TT:15', 'CC:19', 'TC:23'),
      array('Chr4p48765', 'Chr4', '48765', '', 'CC:12', 'AC:7', '', 'CC:19', 'AA:23'),
    ),
  );
  $example = theme('table', $example_table);
  $example .= '<p style="font-size:0.9em; font-style:italic">* The above example will be referred to in the description of each filter criteria to aid in the explanation of how it will affect your data. <strong>NOTE: the cell for each SNP by germplasm combination contains the call and the read depth seperated by a colon (:).</strong> For example, AA:5 means a call of AA with a read depth of 5.</p>';
  $form['s2']['example'] = array(
    '#type' => 'markup',
    '#markup' => $example,
  );

  $form['s3'] = array(
    '#type' => 'fieldset',
    '#title' => 'Pick your Export format.',
    '#description' => 'Select one of the formats listed below and the filtered VCF will be converted accordingly. <strong>Keep in mind that if you choose a format with no quality information, you should have been stringent with your filtering criteria to ensure you are working with good data.</strong>'
  );

  $header = array(
    'name' => 'Format',
    'has_quality' => 'Has Quality Info?',
    'description' => 'Description',
  );
  $options = array(
    'abh' => array(
      'name' => 'ABH Format',
      'has_quality' => 'No',
      'description' => 'Alleles are coded as A if they match the maternal parent, B based on the paternal parent, whereas H represents a heterozygous call and "-" as missing. <br><strong>NOTE:</strong> This format is only suitable for biparental crosses and any SNPs in which the parents are missing, heterozygous, or the same genotypic call will be <strong>excluded</strong>!',
    ),
    'genotype_matrix' => array(
      'name' => 'Genotype Matrix',
      'has_quality' => 'No',
      'description' => 'Genotype by Germplasm matrix.',
    ),
    'qual_matrix' => array(
      'name' => 'Quality Matrix',
      'has_quality' => 'Yes',
      'description' => 'Variant by Germplasm matrix of Read Depth per call.',
    ),
    'vcf' => array(
      'name' => 'Variant Call Format (VCF)',
      'has_quality' => 'Yes',
      'description' => 'A variant by germplasm matrix with each cell containing a combination of SNP call and quality information. See the '.l('Specification', 'http://samtools.github.io/hts-specs/VCFv4.2.pdf').' for more information.',
    ),
    'hapmap' => array(
      'name' => 'Haplotype Map (Hapmap)',
      'has_quality' => 'No',
      'description' => 'A Hapmap file is a tab-separated values(TSV) format for storing genotypic data. Hapmap format is easier to edit and handle but less informative than VCF format.',
    ),
    'bgzipped' => array(
      'name' => 'Bgzipped VCF',
      'has_quality' => 'Yes',
      'description' => 'An archive containing a bgzipped VCF file and a Tabix file. This combination is required by various programs such as the R package VariantAnnotation. See the '.l('tabix manual','http://www.htslib.org/doc/tabix.html').' for more information.',
    ),
  );
  $form['s3']['format'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#multiple' => FALSE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Filter',
  );

  $form['cite'] = array(
    '#type' => 'item',
    '#prefix' => '<div class="citation">',
    '#suffix' => '</div>',
    '#markup' => '<sup>+</sup> <a href="https://academic.oup.com/bioinformatics/article-lookup/doi/10.1093/bioinformatics/btr330">The Variant Call Format and VCFtools</a>, Petr Danecek, Adam Auton, Goncalo Abecasis, Cornelis A. Albers, Eric Banks, Mark A. DePristo, Robert Handsaker, Gerton Lunter, Gabor Marth, Stephen T. Sherry, Gilean McVean, Richard Durbin and 1000 Genomes Project Analysis Group, Bioinformatics, 2011.',
  );

  return $form;
}

/**
 * VCF Filter Form: Validation.
 */
function vcf_filter_form_validate($form, &$form_state) {

  // CHECK: VCF File Required.
  if (empty($form_state['values']['vcf_file_id'])) {
    form_set_error('vcf_file_id', 'You must choose a VCF file to filter.');
  }

  // CHECK: Format required.
  if (empty($form_state['values']['format'])) {
    form_set_error('format', 'You must choose a format to export your filtered VCF file in.');
  }

  // CHECK: Minimum SNP Call Read Depth should be an integer.
  if (!empty($form_state['values']['s2']['call_read_depth'])) {
    if (!ctype_digit($form_state['values']['s2']['call_read_depth'])) {
      form_set_error('s2][call_read_depth', 'The <em>Minimum SNP Call Read Depth</em> should be a positive whole number.');
    }
  }

  // CHECK: Minor Allele Frequency should be between 0-100.
  if (!empty($form_state['values']['s2']['minor_allele_freq']) && ctype_digit($form_state['values']['s2']['minor_allele_freq'])) {
    if ($form_state['values']['s2']['minor_allele_freq'] < 0 || $form_state['values']['s2']['minor_allele_freq'] > 100) {
      form_set_error('s2][minor_allele_freq', 'The <em>Minor Allele Frequency</em> represent a percent and thus should be between 0 and 100 (e.g. "25" for 25%).');
    }
  }
  elseif (!empty($form_state['values']['s2']['minor_allele_freq'])) {
    form_set_error('s2][minor_allele_freq', 'The <em>Minor Allele Frequency</em> should be a positive whole number (e.g. "25" for 25%).');
  }

  // CHECK: Maximum Missing Count is an integer.
  if (!empty($form_state['values']['s2']['max_missing_count'])) {
    if (!ctype_digit($form_state['values']['s2']['max_missing_count'])) {
      form_set_error('s2][max_missing_count', 'The <em>Maximum Missing Count</em> should be a positive whole number.');
    }
  }

  // CHECK: Maximum Missing Frequency should be between 0-100.
  if (!empty($form_state['values']['s2']['max_missing_freq']) && ctype_digit($form_state['values']['s2']['max_missing_freq'])) {
    if ($form_state['values']['s2']['max_missing_freq'] < 0 || $form_state['values']['s2']['max_missing_freq'] > 100) {
      form_set_error('s2][max_missing_freq', 'The <em>Maximum Missing Frequency</em> represent a percent and thus should be between 0 and 100 (e.g. "25" for 25%).');
    }
  }
  elseif (!empty($form_state['values']['s2']['max_missing_freq'])) {
    form_set_error('s2][max_missing_freq', 'The <em>Maximum Missing Frequency</em> should be a positive whole number (e.g. "25" for 25%).');
  }
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
    'qual_matrix' => '/filter_vcf/Quality-Matrix',
    'bgzipped' => '/filter_vcf/Bgzipped',
    'hapmap' => '/filter_vcf/Hapmap',
    'genotype_matrix' => '/filter_vcf/Genotype-Matrix'
    'vcf' => '/filter_vcf/VCF'
  );
  $form_state['redirect'] = array(
    $url[ $form_state['values']['format'] ],
    array(
      'query' => $q
    ),
  );
}

/**
 * AJAX Callback: Update the description of the VCF file.
 */
function ajax_vcf_filter_show_description_callback($form, $form_state) {
  return array($form['s1'], $form['s2']);
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

<?php
/**
 * @file
 * API Functions for this module.
 */

/**
 * Retrieve all VCF files.
 *
 * @return
 *   An array of VCF file details, keyed by vcf_file_id.
 */
function vcf_filter_get_vcf_files() {

  $files = db_query('SELECT * FROM {vcf_files}')->fetchAllAssoc('vcf_file_id', PDO::FETCH_ASSOC);
  return $files;
}

/**
 * Retrieve details for a given VCF file.
 *
 * @param $vcf_file_id
 *   The primary key for the VCF File.
 * @return
 *   An object describing the VCF file.
 */
function vcf_filter_get_vcf_file($vcf_file_id) {

  $file = db_query('SELECT * FROM {vcf_files} WHERE vcf_file_id=:id',
    array(':id' => $vcf_file_id))->fetchObject();

  return $file;
}

/**
 * Filter a VCF file using VCFtools.
 *
 * MUST BE RUN ON THE COMMAND-LINE VIA DRUSH.
 *
 * @param $file_path
 *   The absolute path to the VCF file to be filtered.
 * @param $filter_criteria
 *   An array of filter criteria to be performed on the above VCF file.
 * @return
 *   The full patch of the filtered VCF file.
 */
function vcf_filter_filter_file($file_path, $filter_criteria) {
  $new_file = variable_get('file_temporary_path') . '/' . uniqid();

  drush_log('NOTE: We are not using the PL at this point to specify the genotypes.', 'warning');

  $command = 'vcftools --vcf ' . $file_path . ' --recode --out ' . $new_file;

  // Bi-allelic:
  if ($filter_criteria['bi-allelic'] == 1) {
    $command .= ' --min-alleles 2 --max-alleles 2';
  }

  // Minimum SNP Call Read Depth:
  if (!empty($filter_criteria['call_read_depth'])) {
    $command .= ' --minDP ' . escapeshellarg($filter_criteria['call_read_depth']);
  }

  // Minor Allele Frequency:
  if (!empty($filter_criteria['minor_allele_freq'])) {
    // NOTE: We allow the user to provide the frequency as an integer 0-100;
    // however, vcftools expects a float between 0-1.
    $minor_allele_freq = $filter_criteria['minor_allele_freq'] / 100;
    $command .= ' --maf ' . escapeshellarg($minor_allele_freq);
  }

  // Maximum Missing Count:
  if (!empty($filter_criteria['max_missing_count'])) {
    $command .= ' --max-missing-count ' . escapeshellarg($filter_criteria['max_missing_count']);
  }

  // Maximum Missing Count:
  if (!empty($filter_criteria['max_missing_freq'])) {
    // NOTE: We allow the user to provide the frequency as an integer 0-100;
    // however, vcftools expects a float between 0-1.
    $max_missing_freq = 1 - ($filter_criteria['max_missing_freq'] / 100);
    $command .= ' --max-missing ' . escapeshellarg($max_missing_freq);
  }

  // Execute the command.
  drush_print('Filter results will be in: '.$new_file . '.recode.vcf');
  drush_print(str_repeat('-', 60));
  drush_print(shell_exec($command));
  drush_print(str_repeat('-', 60));

  return $new_file . '.recode.vcf';
}

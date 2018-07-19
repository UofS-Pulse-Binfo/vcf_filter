<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

class DrushUnitTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testAllFormats() {
    $faker = Factory::create();
    $variables_test = array(
      "q" => array(
              "bi-allelic" => 0,
              "call_read_depth" => 5,
              "minor_allele_freq" => 25,
              "max_missing_count" => 10,
              "max_missing_freq" => 25,
              "vcf_file_id" => NULL,
          ),
      "safe_site_name" => "KnowPulse",
      "type_info" => array(
              "type_name" => 'VCF Filter',
              "format" => 'VCF Format',
              "functions" => array(
                      "generate_file" => 'vcf_filter_vcf_generate_file',
              ),
          ),
      "suffix" => 'txt',
      "filename" => 'KnowPulse.vcf_filter_VCF_'.$faker->uuid,
      "fullpath" => DRUPAL_ROOT . '/sites/default/files' . trpdownload_api_get_filedir('full'),
      "relpath" => 'public://tripal/tripal_downloads/',
      "format_name" => 'VCF Format',
  );

  $one_datafile = array(
    "file_path" => DRUPAL_ROOT . '/' . drupal_get_path('module','vcf_filter') . '/tests/test_files/example_file1.txt',
    "name" => $faker->word, // Use $faker here as well
    "num_snps" => 506,
    "backbone" => $faker->word,  // Use $faker here.
    "description" => $faker->text,
  );

  drupal_write_record('vcf_files', $one_datafile);
  $variables_test['q']['vcf_file_id'] = $one_datafile['vcf_file_id'];
  print_r($one_datafile);
  print_r($variables_test);

  $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

  //test format VCF
  vcf_filter_vcf_generate_file($variables_test, NULL, true);
  $this->assertFileExists($result_file_vcf, "VCF format: Result File, $result_file_vcf, does not exist.");
  $this->assertNotEquals(0, filesize($result_file_vcf), "VCF format: The Result File, $result_file_vcf, is empty.");
  unlink($result_file_vcf);

  //test format Hapmap
  $variables_test['type_info']['format'] = 'Hapmap format';
  $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_hapmap_generate_file';
  vcf_filter_hapmap_generate_file($variables_test, NULL, true);
  $this->assertFileExists($result_file_vcf, "Hapmap format: Result File, $result_file_vcf, does not exist.");
  $this->assertNotEquals(0, filesize($result_file_vcf), "Hapmap format: The Result File, $result_file_vcf, is empty.");
  unlink($result_file_vcf);

  //test format bgzipped
  $variables_test['type_info']['format'] = 'bgzipped format';
  $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_bgzipped_generate_file';
  vcf_filter_bgzipped_generate_file($variables_test, NULL, true);
  $this->assertFileExists($result_file_vcf, "Bgzipped format: Result File, $result_file_vcf, does not exist.");
  $this->assertNotEquals(0, filesize($result_file_vcf), "Bgzipped format: The Result File, $result_file_vcf, is empty.");
  unlink($result_file_vcf);



  }
}

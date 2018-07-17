<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

class DrushUnitTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testVcfFormat() {
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
      "fullpath" => '/var/www/dev/cloned-clone/sites/default/files/tripal/tripal_downloads/',
      "relpath" => 'public://tripal/tripal_downloads/',
      "format_name" => 'VCF Format',
  );

  $one_datafile = array(
    "file_path" => drupal_get_path('module','vcf_filter') . '/tests/test_files/example_file1.txt',
    "name" => 'test_file1', // Use $faker here as well
    "num_snps" => 506,
    "backbone" => 'Test Backbone',  // Use $faker here.
    "description" => 'This is a a test file containing 506 SNPs.', // Use $faker here
  );
  drupal_write_record('vcf_files', $one_datafile);
  $variables_test['q']['vcf_file_id'] = $one_datafile['vcf_file_id'];
  print_r($one_datafile);

  $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

  vcf_filter_vcf_generate_file($variables_test, NULL, true);

  $this->assertFileExists($result_file_vcf, "Result File, $result_file_vcf, does not exist.");

  $this->assertNotEquals(0, filesize($result_file_vcf), "The Result File, $result_file_vcf, is empty.");

  }
}

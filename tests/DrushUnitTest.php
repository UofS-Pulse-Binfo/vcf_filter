<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
use Faker\Factory;

class DrushUnitTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;
  /**
   * prepare all variables we need for test
   * using faker to generate names and IDs
   * benefit of using __construct is each test can use a file with unique ID, even with same test file
   */
  public function generate_test_file(){
    $faker = Factory::create();
    $variables = array(
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

    $datafile_fake = array(
        "file_path" => DRUPAL_ROOT . '/' . drupal_get_path('module','vcf_filter') . '/tests/test_files/example_file1.txt',
        "name" => $faker->word, // Use $faker here as well
        "num_snps" => 506,
        "backbone" => $faker->word,  // Use $faker here.
        "description" => $faker->text,
    );
    drupal_write_record('vcf_files', $datafile_fake);
    $variables['q']['vcf_file_id'] = $datafile_fake['vcf_file_id'];
    print $variables["filename"] . ' is generated in path ' . $variables["fullpath"] . ' with file ID: ' . $variables['q']['vcf_file_id'] . "\n";
    return $variables;
    //return array($variables, $datafile_fake);
  }

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testVCFFormat() {
    $variables_test = $this -> generate_test_file();
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];
    //test format VCF
    vcf_filter_vcf_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "VCF format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "VCF format: The Result File, $result_file_vcf, is empty.");
    unlink($result_file_vcf);
  }

  public function testHapMapFormat() {
    //test format Hapmap
    $variables_test = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'Hapmap format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_hapmap_generate_file';
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    vcf_filter_hapmap_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Hapmap format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Hapmap format: The Result File, $result_file_vcf, is empty.");
    unlink($result_file_vcf);
  }

  public function testBgzippedFormat() {
    //test format bgzipped
    $variables_test = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'bgzipped format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_bgzipped_generate_file';
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    vcf_filter_bgzipped_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Bgzipped format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Bgzipped format: The Result File, $result_file_vcf, is empty.");
    unlink($result_file_vcf);
  }

  public function testABHFormat() {
    //test format ABH
    $variables_test = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'ABH format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_abh_generate_file';
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    vcf_filter_abh_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Bgzipped format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Bgzipped format: The Result File, $result_file_vcf, is empty.");
    unlink($result_file_vcf);
  }

  public function testQualMatrixFormat() {
    //test format qual_matrix
    $result_file_vcf = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'qual_matrix format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_qual_matrix_generate_file';
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    vcf_filter_qual_matrix_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Bgzipped format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Bgzipped format: The Result File, $result_file_vcf, is empty.");
    unlink($result_file_vcf);
  }

}

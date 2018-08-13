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
   * benefit of using this function is each test using a file with unique ID, even with same test file
   */
  public function generate_test_file(){
    $faker = Factory::create();
    $variables = array(
        "q" => array(
            "bi-allelic" => 0,
            "call_read_depth" => $faker->numberBetween(2 ,10),
            "minor_allele_freq" => $faker->numberBetween(35 ,45),
            "max_missing_count" => $faker->numberBetween(1 ,10),
            "max_missing_freq" => $faker->numberBetween(20 ,30),
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
    $variables['q']['vcf_file_id'] = $one_datafile['vcf_file_id'];
    return $variables;
  }

  /**
   * @section
   * Tests for different functions(formats)
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   * reasons to split into different test functions are:
   *  1. each time test_file can be regenerated with unique id, and random parameters for VcfTools
   *  2. maybe good for more specific and expanded tests for each format next
   */

  /**
   * test VCF format
   * Specifically testing vcf_filter_vcf_generate_file():
   *  - result file exists
   *  - result file is not empty
  */
  public function testVCFFormat() {
    //import test file
    $variables_test = $this -> generate_test_file();
    print "\n\n\n\nStarting test for VCF format with: \n" . "Filename: " . $variables_test["filename"] . "\nFullpath: " . $variables_test["fullpath"] . "\nFile ID: " . $variables_test['q']['vcf_file_id'] . "\n";
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    //execute function and do tests
    vcf_filter_vcf_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "VCF format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "VCF format: The Result File, $result_file_vcf, is empty.");

    //remove files that won't be used anymore
    unlink($result_file_vcf);
  }

  /**
   * test Hapmap format
   * Specifically testing vcf_filter_hapmap_generate_file():
   *  - result file exists
   *  - result file is not empty
  */
  public function testHapMapFormat() {
    //import test file and update variables
    $variables_test = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'Hapmap format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_hapmap_generate_file';

    //print varialbes on screen to show test processes
    print "\n\n\nStarting test for Hapmap format with: \n" . "Filename: " . $variables_test["filename"] . "\nFullpath: " . $variables_test["fullpath"] . "\nFile ID: " . $variables_test['q']['vcf_file_id'] . "\n";
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    //execute function and do tests
    vcf_filter_hapmap_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Hapmap format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Hapmap format: The Result File, $result_file_vcf, is empty.");

    //remove files that won't be used anymore
    unlink($result_file_vcf);
  }

  /**
   * test Bgzipped format
   * Specifically testing vcf_filter_bgzipped_generate_file():
   *  - result file exists
   *  - result file is not empty
  */
  public function testBgzippedFormat() {
    //import test file and update variables
    $variables_test = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'bgzipped format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_bgzipped_generate_file';

    //print varialbes on screen to show test processes
    print "\n\n\nStarting test for Bgzipped format with: \n" . "Filename: " . $variables_test["filename"] . "\nFullpath: " . $variables_test["fullpath"] . "\nFile ID: " . $variables_test['q']['vcf_file_id'] . "\n";
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    //execute function and do tests
    vcf_filter_bgzipped_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Bgzipped format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Bgzipped format: The Result File, $result_file_vcf, is empty.");

    //remove files that won't be used anymore
    unlink($result_file_vcf);
  }

  /**
   * test ABH format
   * Specifically testing vcf_filter_abh_generate_file():
   *  - result file exists
   *  - result file is not empty
  */
  public function testABHFormat() {
    //import test file and update variables
    $variables_test = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'ABH format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_abh_generate_file';

    //print varialbes on screen to show test processes
    print "\n\n\nStarting test for ABH format with: \n" . "Filename: " . $variables_test["filename"] . "\nFullpath: " . $variables_test["fullpath"] . "\nFile ID: " . $variables_test['q']['vcf_file_id'] . "\n";
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    //execute function and do tests
    vcf_filter_abh_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Bgzipped format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Bgzipped format: The Result File, $result_file_vcf, is empty.");

    //remove files that won't be used anymore
    unlink($result_file_vcf);
  }

  /**
   * test Quality Matrix format
   * Specifically testing vcf_filter_qual_matrix_generate_file():
   *  - result file exists
   *  - result file is not empty
  */
  public function testQualMatrixFormat() {
    //import test file and change some variables
    $variables_test = $this -> generate_test_file();
    $variables_test['type_info']['format'] = 'qual_matrix format';
    $variables_test['type_info']['functions']['generate_file'] = 'vcf_filter_qual_matrix_generate_file';

    //print varialbes on screen to show test processes
    print "\n\n\nStarting test for Qual_matrix format with: \n" . "Filename: " . $variables_test["filename"] . "\nFullpath: " . $variables_test["fullpath"] . "\nFile ID: " . $variables_test['q']['vcf_file_id'] . "\n";
    $result_file_vcf = $variables_test['fullpath'].$variables_test['filename'];

    //execute function and do tests
    vcf_filter_qual_matrix_generate_file($variables_test, NULL, true);
    $this->assertFileExists($result_file_vcf, "Bgzipped format: Result File, $result_file_vcf, does not exist.");
    $this->assertNotEquals(0, filesize($result_file_vcf), "Bgzipped format: The Result File, $result_file_vcf, is empty.");

    //remove files that won't be used anymore
    unlink($result_file_vcf);
  }

}

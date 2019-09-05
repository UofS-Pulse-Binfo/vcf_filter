<?php
namespace Tests\Formats;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class HAPMAPTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Tests the HAPMAP Format with no filtering.
   * @group formats
   * @group hapmap
   */
  public function testHAPMAP() {

    // 1) Run the conversion.
    // -- make a copy of my inout VCF so it doesn't get automatically cleaned up.
    $original_input_file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'vcf_filter') . '/tests/test_files/short.vcf';
    $input_file = file_directory_temp() . '/' . uniqid() . '.txt';
    copy($original_input_file, $input_file);
    // -- Set a unique output file name.
    $output_file = file_directory_temp() . '/' . uniqid() . '.txt';
    // -- Run the conversion.
    vcf_filter_convert_VCF_to_Hapmap(
      $input_file,
      $output_file
    );

    // 2) Ensure the output file matches what we expected.
    $expected_file = DRUPAL_ROOT . '/' . drupal_get_path('module', 'vcf_filter') . '/tests/test_files/short.hapmap.txt';
    $this->assertFileEquals($expected_file, $output_file,
      "The resulting file, $output_file, doesn't match what we expected.");
  }
}

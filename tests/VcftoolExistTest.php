<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class VcftoolExistTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testBasicExample() {
    $vcftools_output = exec('vcftools --version');
    $this->assertTrue(preg_match("/VCFtools/", $vcftools_output));
  }
}

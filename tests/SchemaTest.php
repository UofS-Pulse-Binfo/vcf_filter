<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class SchemaTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  /**
   * check installation of this module
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   *
   * what we need to check (schema):
   * -  vcf_files
   * -  vcf_files_perm
   */
  public function testSchemaExist() {
    $tables = array(
      'vcf_files',
      'vcf_files_perm',
    );
    foreach($tables as $one_table){
      $exists = db_table_exists($one_table);
      $this -> assertTrue($exists, "Checking that $one_table table exists.");
    }
  }

}

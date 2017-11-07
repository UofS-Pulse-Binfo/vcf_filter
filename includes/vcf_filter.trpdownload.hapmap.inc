<?php
/**
 * @file
 * Handles download of filtered files in Hapmap format.
 */
/**
 * Voncersion from filtered VCF file to hapmap file.
 *
 * @param $variables
 *   An associative array of parameters including:
 *     - q: all the query paramters.
 *     - site_safe_name: a sanitized version of your site name for use in variables & filenames.
 *     - type_info: an array of info for the download type.
 *     - suffix: the file format suffix.
 *     - filename: the filename of the file to generate not including path.
 *     - fullpath: the full path and filename of the file to generate.
 *     - format_name: a human-readable description of the format.
 * @param $job_id
 *   The ID of the tripal job executing this function ;-).
 */
function vcf_filter_hapmap_generate_file($variables, $job_id = NULL) {
  // Create the file and ready it for writting to.
  $filepath = variable_get('trpdownload_fullpath', '') . $variables['filename'];
  drush_print("File: " . $filepath);
  // Get the VCF file details.
  $vcf_file = vcf_filter_get_vcf_file($variables['q']['vcf_file_id']);
  drush_print("\nVCF File (".$vcf_file->name."): ".$vcf_file->file_path);
  // Check the input file still exists.
  if (!file_exists($vcf_file->file_path)) {
    drush_set_error("ERROR: Original VCF file not found!");
    exit(2);
  }
  // Print out parameters.
  drush_print("Parameters:");
  foreach ($variables['q'] as $label => $value) {
    drush_print('  ' . $label . ': ' . $value);
  }
  // Give the user some indication we've done something...
  tripal_set_job_progress($job_id, 5);
  // Using the parameters user gave to filter vcf file
  //=====================================================
  drush_print("\nFiltering the Original VCF");
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q']);
  if (!$filtered_vcf) {
    exit(3);
  }
  // Half way point...
  tripal_set_job_progress($job_id, 50);
  // Ready for next step: FORMAT. 
  // Convert filtered vcf file to format user expected (Hapmap in this case). 
  //=====================================================

  //------------------------------------------------------------------
  // new script starts here, above codes are copied from old script with minor changes
  // the extension of hapmap format filename can to be '.hmp' 
  drush_print("\nConversion from VCF to Hapmap Format.");
  $format_tmp = variable_get('file_temporary_path') . '/' . uniqid();

  $outfile = $format_tmp . '.hmp';
  drush_print('Converted results will be in: '.$outfile);
  drush_print(str_repeat('-', 60));

  //use function converting filter vcf filt to hapmap format
  vcf_filter_convert_VCF_to_Hapmap($filtered_vcf, $outfile);

  if (!file_exists($outfile)) {
    drush_set_error("ERROR: Unable to convert VCF file to hapmap format.");
    exit(4); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
  }

  // Tell the user we're almost there!
  tripal_set_job_progress($job_id, 90);
  
  drush_print("\nMove Formatted file to the name we expect.");
  shell_exec("mv $format_tmp.hmp $filepath");
  if (!file_exists($filepath)) {
    drush_set_error("ERROR: Unable to move hapmap file into expected file director: $filepath");
    exit(5); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
  }

  // Done!
  tripal_set_job_progress($job_id, 100);  

}

/**
 * This script will take any spec-conforming VCF file and convert it to a hapmap format file
 *
 *
 *  @param $in_vcf_filename
 *    A tab-delimited text file in Variant Call Format (VCF). The spec for VCF files can be found here:
 *    http://samtools.github.io/hts-specs/
 *  @param $out_hapmap_filename
 *    A name to give the final Hapmap format output file.
*/

function vcf_filter_convert_VCF_to_Hapmap($in_vcf_filename, $out_hapmap_filename) {

  // create output file
  $handle_out = fopen($out_hapmap_filename,'w') or die ("\nCannot create file: $out_hapmap_filename .\n");

  // read vcf file by lines
  foreach (new SplFileObject($in_vcf_filename) as $line) {
    trim ($line);
    // if one line starts with '#CHROM' => header
    // if one line doesn't start with '#' and not empty => one SNP line
    if (preg_match('/^#CHROM/', $line)) {
      $headerline = explode("\t", $line);
      fwrite($handle_out, ("rs#\talleles\tchrom\tpos\tstrand\tassembly#\tcenter\tprotLSID\tassayLSID\tpanel\tQCcode"));
      for ($i = 9; $i < count($headerline); $i++) {
        // tassel can not sepeare tabs and spaces in germplasm names, so we have to replace spaces with underscores for tassel
        // only for hapmap, the problem doesn't exit for vcf files
        $headerline[$i] = str_replace(' ', '_', $headerline[$i]);
        fwrite($handle_out, "\t$headerline[$i]"); 
      }
    }
  
    if ((!preg_match('/^#/', $line)) && (!empty($line))) {
      $splitline = explode("\t", $line);

      fwrite($handle_out, ($splitline[0] . 'p' . $splitline[1]."\t". $splitline[3]. '/'. $splitline[4]));

      fwrite($handle_out, ("\t". $splitline[0]. "\t". $splitline[1]. "\tNA\tNA\tNA\tNA\tNA\tNA\tNA"));
      
      // merge the reference and alternates(1-3) genotypes into one array $gt_ref_min
      // corresponding $gt_ref_min[0] for reference gt, $gt_ref_min[1] for first alternate and so on
      $gt_ref_min = array_merge(array("$splitline[3]"), explode(',',$splitline[4]));

      // 2 loops: first by samples (9th column to end)
      // second by number of GT(genotype)(e.g.:0/0) in FORMAT(e.g.: 0/0:0,36,89:51:5:40) of vcf file
      // at first explod 0/0:0,36,89:51:5:40 into array("0/0","0,36,89",...,"40") then explode first element into new array("0", "0")
      // then match "0" for reference gt, "1" for first alternate and so on

      for ($m = 9; $m < count($headerline); $m++) {
        $call = explode(":", $splitline[$m]);

        // should explode both by '/' and '|' for both unpashed and pahsed
        $call_gt = explode('/', str_replace("|","/",$call[0]));
        
        fwrite($handle_out, "\t");

        for ($n=0; $n < count($call_gt); $n++) {
          if (strcmp($call_gt[$n], ".") == 0) {
            fwrite($handle_out, 'N');
          }
          elseif (is_numeric($call_gt[$n]) == true) {
            fwrite($handle_out, $gt_ref_min[$call_gt[$n]]);
          }
          else {
            die ("ERROR: invalid genotype call at marker position ". $splitline[0]. " ". $splitline[1]. "\n");
          }
        }
      }
      fwrite($handle_out, ("\n"));
    }
  }
  fclose($handle_out);
}

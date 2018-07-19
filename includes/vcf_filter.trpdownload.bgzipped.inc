<?php
/**
 * @file
 * Handles download of VCF files in bgzipped Format for use in R package (VariantAnnotation).
 */
/**
 * Generate bgzipped File from VCF.
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
 * @param $drush_use_limited
 *   are drush function limited
 */
function vcf_filter_bgzipped_generate_file($variables, $job_id = NULL, $drush_use_limited = false) {
  // Create the file and ready it for writting to.
  $filepath = $variables['fullpath'] . $variables['filename'];
  print "File: ".$filepath."\n";
  // Get the VCF file details.
  $vcf_file = vcf_filter_get_vcf_file($variables['q']['vcf_file_id']);
  print "\nVCF File (".$vcf_file->name."): ".$vcf_file->file_path."\n";
  // Check the input file still exists.
  if (!file_exists($vcf_file->file_path)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Original VCF file not found!");
      exit(2);
    }else{
      print "ERROR: Original VCF file not found!\n";
      exit(2);
    }
  }
  // Print out parameters.
  print "Parameters:\n";
  foreach ($variables['q'] as $label => $value) {
    print '  ' . $label . ': ' . $value."\n";
  }
  // Give the user some indication we've done something...
  tripal_set_job_progress($job_id, 5);
  // Filter.
  //===============
  print "\nFiltering the Original VCF\n";
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q']);
  if (!$filtered_vcf) {
    exit(3);
  }
  // Sort filtered vcf file for use in Tassel
  print "\nSorting the Original VCF\n";
  $filtered_sorted_vcf = vcf_filter_sort_filtered_file($filtered_vcf);
  if (!$filtered_sorted_vcf) {
    exit(4);
  }

  // Half way point...
  tripal_set_job_progress($job_id, 50);
  // FORMAT.
  //===============
  print "\nConversion from VCF to bgzipped VCF Format.\n";

  //remove the extension of filename we can obtain from $variables
  //since we know the extension is always '.vcf', to be safe, no longer need to remove last 3,4 characters
  //$bgzip_filename = preg_replace('/\\.[^.\\s]{3,4}$/', '', $variables['filename']);
  $bgzip_filename = preg_replace('/\.tar$/', '', $variables['filename']);

  $format_tmp = $variables['fullpath'] . '/' . $bgzip_filename;
  $outfile = $format_tmp . '.tar';

  /*Creating a compressed and indexed VCF in 3 steps
    1. bgzip -c file.vcf > file.vcf.gz
    2. tabix -p vcf file.vcf.gz
    3. tar cvf archive.tar file.vcf.gz file.vcf.gz.tbi
  */

  $bgzip_tmp_path = variable_get('file_temporary_path');

  print 'Using bgzip to generate bgzipped file: '.$format_tmp.'.gz'."\n";
  $command = 'bgzip -c ' . escapeshellarg($filtered_sorted_vcf) . ' > ' . escapeshellarg($format_tmp) . '.gz';
  shell_exec($command);

  print 'Using tabix to generate index file: '.$format_tmp.'.gz.tbi'."\n";
  $command = 'tabix -p vcf ' . escapeshellarg($format_tmp) . '.gz';
  shell_exec($command);

  print 'Combining into an archive: '.$format_tmp.'.tar'."\n";
  $command = 'tar cvf '.escapeshellarg($format_tmp).'.tar'.' -C '.escapeshellarg($bgzip_tmp_path).' '.escapeshellarg($bgzip_filename).'.gz '.escapeshellarg($bgzip_filename).'.gz.tbi';
  shell_exec($command);

  if (!file_exists($outfile)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Unable to convert VCF file to an archive file (.tar).");
      exit(5); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
    }else{
      print "ERROR: Unable to convert VCF file to an archive file (.tar).\n";
      exit(5);
    }
  }

  // Tell the user we're almost there!
  tripal_set_job_progress($job_id, 90);

  print "Move formatted file to the name we expect.\n";
  shell_exec("mv $format_tmp.tar $filepath");

  if (!file_exists($filepath)) {
    if ($drush_use_limited === false){
      drush_set_error("ERROR: Unable to move Filtered VCF into expected file director: $filepath");
      exit(6); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
    }else{
      print "ERROR: Unable to move Filtered VCF into expected file director: $filepath\n";
      exit(6);
    }

  }
  // Done!
  tripal_set_job_progress($job_id, 100);
}

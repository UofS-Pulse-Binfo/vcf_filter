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
 */
function vcf_filter_bgzipped_generate_file($variables, $job_id = NULL) {
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
  // Filter.
  //===============
  drush_print("\nFiltering the Original VCF");
  $filtered_vcf = vcf_filter_filter_file($vcf_file->file_path, $variables['q']);
  if (!$filtered_vcf) {
    exit(3);
  }
  // Sort filtered vcf file for use in Tassel
  drush_print("\nSorting the Original VCF");
  $filtered_sorted_vcf = vcf_filter_sort_filtered_file($filtered_vcf);
  if (!$filtered_sorted_vcf) {
    exit(4);
  }

  // Half way point...
  tripal_set_job_progress($job_id, 50);
  // FORMAT.
  //===============
  drush_print("\nConversion from VCF to bgzipped VCF Format.");

  //remove the extension of filename we can obtain from $variables
  //since we know the extension is always '.vcf', to be safe, no longer need to remove last 3,4 characters
  //$bgzip_filename = preg_replace('/\\.[^.\\s]{3,4}$/', '', $variables['filename']);
  $bgzip_filename = preg_replace('/\.tar$/', '', $variables['filename']);
  
  $format_tmp = variable_get('file_temporary_path') . '/' . $bgzip_filename;
  $outfile = $format_tmp . '.tar';
  
  /*Creating a compressed and indexed VCF in 3 steps
    1. bgzip -c file.vcf > file.vcf.gz
    2. tabix -p vcf file.vcf.gz
    3. tar cvf archive.tar file.vcf.gz file.vcf.gz.tbi
  */
 
  $bgzip_tmp_path = variable_get('file_temporary_path');
  
  drush_print('Using bgzip to generate bgzipped file: '.$format_tmp.'.gz');
  $command = 'bgzip -c ' . escapeshellarg($filtered_sorted_vcf) . ' > ' . escapeshellarg($format_tmp) . '.gz';
  drush_print(shell_exec($command));
  
  drush_print('Using tabix to generate index file: '.$format_tmp.'.gz.tbi');
  $command = 'tabix -p vcf ' . escapeshellarg($format_tmp) . '.gz';
  drush_print(shell_exec($command));
  
  drush_print('Combining into an archive: '.$format_tmp.'.tar');
  $command = 'tar cvf '.escapeshellarg($format_tmp).'.tar'.' -C '.escapeshellarg($bgzip_tmp_path).' '.escapeshellarg($bgzip_filename).'.gz '.escapeshellarg($bgzip_filename).'.gz.tbi';
  drush_print(shell_exec($command));
  
  if (!file_exists($outfile)) {
    drush_set_error("ERROR: Unable to convert VCF file to an archive file (.tar).");
    exit(5); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
  }
  
  // Tell the user we're almost there!
  tripal_set_job_progress($job_id, 90);  
  
  drush_print("Move formatted file to the name we expect.");
  shell_exec("mv $format_tmp.tar $filepath");
  
  if (!file_exists($filepath)) {
    drush_set_error("ERROR: Unable to move Filtered VCF into expected file director: $filepath");
    exit(6); // We use exit to trigger a Tripal job error b/c Tripal doesn't check the return code :-(.
  }
  // Done!
  tripal_set_job_progress($job_id, 100);
}

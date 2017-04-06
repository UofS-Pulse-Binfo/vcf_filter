# VCF Filter
Tripal form interface so users can custom filter existing VCF files and export in a variety of formats. The form simply provides an interface to VCFtools and uses the Tripal Download API to provide the filtered file to the user.

## Features
- User "Filter VCF" form providing well documented filter options (includes examples) and a variety of formats.
  - Filter Options include: Only bi-allelic SNPs, Minimum SNP Call Read Depth, Minor Allele Frequency, Maximum Missing Count, Maximum Missing Frequency.
  - Export Formats include: VCF, SNP Matrix (Genotype only), Quality Matrix (read depth only), A/B Biparental Matrix.
- All filtering and format conversion is done within a Tripal Job to support large files.
- Administrative interface for exposing VCF files to users. In addition to specifying the path to the VCF file to expose, record helpful information like a friendly name, assembly aligned to, number of SNPs, etc.
- Per VCF file permissions allowing you to restrict access to a given file to specific users or roles.

## Dependencies
- Tripal Core (utilizes the Tripal API)
- Tripal Donwload API

**NOTE: Compatible with both Tripal 2.x and 3.x.**

## Installation
1. Install Dependencies.
2. Install VCFtools on the same server as Drupal and ensure it is in $PATH.
3. Install this module as you would any other Drupal/Tripal module.

## Usage
1. Describe each VCF file you would like to make available at Administration > Tripal > Tripal Extensions > VCF Filter.
2. Once the file is described, click "access" and make it available to the desired group of users.
3. Point users at the filter form: [your drupal site]/filter_vcf

## Screenshots
The following screenshot shows the user filter form. For more screenshots, look in the "screenshots" directory in this repository.
![screenshot of user filter form](https://github.com/UofS-Pulse-Binfo/vcf_filter/blob/master/screenshots/screencapture-vcf_filter-user_form.png)

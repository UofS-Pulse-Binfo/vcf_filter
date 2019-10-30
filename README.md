[![Build Status](https://travis-ci.org/UofS-Pulse-Binfo/vcf_filter.svg?branch=master)](https://travis-ci.org/UofS-Pulse-Binfo/vcf_filter)

# VCF Filter

This module provides a form interface so users can custom filter existing VCF files and export in a variety of formats. The form simply provides an interface to VCFtools and uses the Tripal Download API to provide the filtered file to the user.

## Dependencies
- Tripal Core (utilizes the Tripal API)
- [Tripal Download API](https://github.com/tripal/trpdownload_api)

**NOTE: Compatible with both Tripal 2.x and 3.x.**

## Installation
1. Install Dependencies.
2. Install VCFtools on the same server as Drupal and ensure it is in $PATH.
3. Install this module as you would any other Drupal/Tripal module.

## Documentation
Please visit our [online documentation](https://vcf-filter.readthedocs.io/en/latest/) to learn more about this module.

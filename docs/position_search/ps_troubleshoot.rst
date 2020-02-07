Troubleshoot
============

If VCF Position Search is not working as expected, please check:

- if bcftools works properly

- if vcf files can be downloaded properly in VCF Bulk Loader

- if compressed vcf file (your_file.vcf.gz) and index file (your_file.vcf.gz.tbi or your_file.vcf.gz.csi) are provided in right directory

- if your compressed vcf files remain good integrity by:

  .. code:: bash

    bcftools view your_file.vcf.gz

- if one variant (e.g.: 19:111) is searchable by:

  .. code:: bash

    cd directory_includes_vcf_fiels

    bcftools view --no-header -r 19:111 your_file.vcf.gz

.. note::

  If you have any questions or suggestions, please contacts us at knowpulse@usask.ca.

Features
========
- User "Filter VCF" form providing well documented filter options (includes examples) and a variety of formats.
    - Basic filter options include: Only bi-allelic SNPs, Minimum SNP Call Read Depth, Minor Allele Frequency, Maximum Missing Count, Maximum Missing Frequency.
    - More filter options include: regions and germplasm.
    - Export Formats include: VCF, Quality Matrix (read depth only), A/B Biparental Matrix, Hapmap, Bgzipped VCF.

- All filtering and format conversion is done within a Tripal Job to support large files.

- Administrative interface for exposing VCF files to users. Extensive configuration options allow comprehensive description of each VCF file, which can offer great user experience.
    - In addition to specifying the path to the VCF file to expose, record helpful information like a friendly name, assembly aligned to, number of SNPs.
    - The information of the methods used in generating each VCF file, a statistic summary and more description can be included.
    - All germplasm names and Chromosome name format can be included as more helpful information.

- Per VCF file permissions allowing you to restrict access to a given file to specific users or roles.


.. toctree::
   :maxdepth: 2
   :caption: Further Detail:

   features/filter_options
   features/configuration_options
   features/restrict_access

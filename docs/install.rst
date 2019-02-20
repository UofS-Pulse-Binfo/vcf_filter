Installation
============

Download VCF Filter
-------------------

The module is availabe as ----. Recommended method of downloading and installation is using Drush:

.. code:: bash

  cd [your drupal root]/sites/all/modules
  git clone

The commands from above will download the module into the specified directory. And the command for installation is:

.. code:: bash

  drush pm-enable vcf_filter


We should able to find this module in "Home » Administration » Tripal » Extensions".

.. image:: install.1.menubar.png

We need to check dependencies before we can enable it for our site.


Dependencies
------------

- Tripal Core (utilizes the Tripal API)
- Tripal Donwload API

.. image:: install.2.dependency.png

From the example above, it is clear that Trpdownload_api is required but missing.

.. code:: bash
  cd [your drupal root]/sites/all/modules
  git clone



Enable VCF Filter
-----------------

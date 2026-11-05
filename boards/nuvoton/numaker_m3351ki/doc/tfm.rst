Build TF-M enabled application
##############################

This document guides how to build `TF-M`_ enabled application.

.. _TrustedFirmware-M: https://www.trustedfirmware.org/projects/tf-m/
.. _TF-M: `TrustedFirmware-M`_

Updating source code
********************

The `TF-M`_ support doesn't upstream yet.
Users need to switch to specific branches for related repositories.

For ``zephyr`` repository, switch to the ``nvt_m3351_enable_tfm`` branch:

.. code-block:: console

    $ cd `west topdir`
    $ cd zephyr
    $ git remote add opennuvoton https://github.com/OpenNuvoton/zephyr
    $ git fetch opennuvoton
    $ git checkout -b nvt_m3351_enable_tfm opennuvoton/nvt_m3351_enable_tfm

Update west modules:

.. code-block:: console

    $ cd `west topdir`
    $ west update

For ``trusted-firmware-m`` repository, switch to the ``nuvoton_zephyr_m3351`` branch:

.. code-block:: console

    $ cd `west topdir`
    $ cd modules/tee/tf-m/trusted-firmware-m
    $ git remote add opennuvoton https://github.com/OpenNuvoton/trusted-firmware-m
    $ git fetch opennuvoton
    $ git checkout -b nuvoton_zephyr_m3351 opennuvoton/nuvoton_zephyr_m3351

For ``hal_nuvoton`` repository, switch to the ``master`` branch:

.. code-block:: console

    $ cd `west topdir`
    $ cd modules/hal/nuvoton
    $ git fetch upstream
    $ git checkout -b master upstream/master

Zephyr board options
********************

The board options are summarized below:

+----------------------------+-------------------------------------------+
| Board                      | Description                               |
+============================+===========================================+
| numaker_m3351ki/m335xxx    | Building with TrustZone disabled          |
+----------------------------+-------------------------------------------+
| numaker_m3351ki/m335xxx/ns | Building with TrustZone/TF-M enabled      |
+----------------------------+-------------------------------------------+

Board: numaker_m3351ki/m335xxx
==============================

The ``numaker_m3351ki/m335xxx`` board target is used to build
TrustZone disabled application as normal.

Board: numaker_m3351ki/m335xxx/ns
=================================

The ``numaker_m3351ki/m335xxx/ns`` board target is used to build
TrustZone enabled application.
It will build `TrustedFirmware-M`_ as Secure image
(``CONFIG_BUILD_WITH_TFM`` to ``y``).
and Zephyr application as Non-secure image
(``CONFIG_TRUSTED_EXECUTION_NONSECURE`` to ``y``).

Memory and Peripheral Ownership
*******************************

Partitioning APROM
==================

APROM is 1024KB in total.
It is partitioned for Secure and Non-secure.
Its address map:

+---------------------+-------------------+------------------------------------+
| Name                | Address[Size]     | Comment                            |
+=====================+===================+====================================+
| APROM Secure        | 0x0[512KB]        | APROM partitioned for Secure       |
+---------------------+-------------------+------------------------------------+
| APROM Non-secure    | 0x10080000[512KB] | APROM partitioned for Non-secure   |
+---------------------+-------------------+------------------------------------+

More detailed address map:

+----------------------+-------------------+------------------------------------+
| Name                 | Address[Size]     | Comment                            |
+======================+===================+====================================+
| BL2/MCUboot          | 0x0[64KB]         | MCUboot secure image               |
+----------------------+-------------------+------------------------------------+
|                      | 0x10000[64KB]     | Reserved                           |
+----------------------+-------------------+------------------------------------+
| Secure Primary       | 0x20000[192KB]    | Primary slot of secure image       |
+----------------------+-------------------+------------------------------------+
| Secure Secondary     | 0x50000[192KB]    | Secondary slot of secure image     |
+----------------------+-------------------+------------------------------------+
| Non-secure Primary   | 0x10080000[256KB] | Primary slot of non-secure image   |
+----------------------+-------------------+------------------------------------+
| Non-secure Secondary | 0x100c0000[256KB] | Secondary slot of non-secure image |
+----------------------+-------------------+------------------------------------+

Partitioning LDROM
==================

LDROM is 16KB in total.
It is partitioned for Secure wholly.
Its address map:

+----------------------+------------------+------------------------------------+
| Name                 | Address[Size]    | Comment                            |
+======================+==================+====================================+
|                      | 0x0f100000[8KB]  | Unused                             |
+----------------------+------------------+------------------------------------+
| MCUboot Scratch Area | 0x0f102000[8KB]  |                                    |
+----------------------+------------------+------------------------------------+

Partitioning Data Flash
=======================

Data Flash is 64KB in total.
It is partitioned for Secure wholly.
Its address map:

+--------------------------------+-------------------+------------------------------------+
| Name                           | Address[Size]     | Comment                            |
+================================+===================+====================================+
| Protected Storage (PS)         | 0x0f210000[24KB]  | Unused                             |
+--------------------------------+-------------------+------------------------------------+
| Internal Trusted Storage (ITS) | 0x0f216000[24KB]  |                                    |
+--------------------------------+-------------------+------------------------------------+
| OTP / NV Counters              | 0x0f21c000[16KB]  |                                    |
+--------------------------------+-------------------+------------------------------------+

Partitioning SRAM
=================

SRAM is 128KB in total.
It is partitioned for Secure and Non-secure.
Its address map:

+----------------------+------------------+------------------------------------+
| Name                 | Address[Size]    | Comment                            |
+======================+==================+====================================+
| SRAM Secure          | 0x20000000[64KB] | SRAM partitioned for Secure        |
+----------------------+------------------+------------------------------------+
| SRAM Non-secure      | 0x30010000[64KB] | SRAM partitioned for Non-secure    |
+----------------------+------------------+------------------------------------+


Partitioning Peripherals
========================

- Always Secure peripherals: CRYPTO, WDT0, and WWDT0
- Secure peripherals with shared registers: CLK, DFMC, FMC, NMI, SCU, and SYS
- Partitioned for Secure: CACHE
- All other peripherals are partitioned for Non-secure.

.. note::

   UART0 is partitioned for Non-secure. However, it is used for console for both
   Secure and Non-secure.

Building and Flashing
*********************

Building
========

To build `TF-M`_ enabled application, specify board target
as ``numaker_m3351ki/m335xxx/ns``:

.. code-block:: console

    $ west -v build \
    -b numaker_m3351ki/m335xxx/ns \
    zephyr/samples/hello_world

Flashing
========

For `TF-M`_ enabled build, this will flash ``tfm_merged.hex``
which is composed of 3 images:

- MCUboot (Secure)
- TF-M (Secure)
- Zephyr (Non-secure)

.. code-block:: console

    $ west flash

Troubleshooting
***************

``west flash`` is failed
========================

Error message::

   -- west flash: using runner pyocd
   Flashing file: <path-to-tfm_merged.hex>
   0001558 C Target type m3351kjcae not recognized. Use 'pyocd list --targets' to see currently available target types. See <https://pyocd.io/docs/target_support.html> for how to install additional target support. [__main__]
   FATAL ERROR: command exited with status 1: pyocd flash -e sector -a 0x10080000 -t m3351kjcae '<path-to-tfm_merged.hex>'

Zephyr invokes pyOCD to download built binary.
PyOCD doesn’t have built-in support for M3351 series MCU.
Add the support via `Nuvoton’s CMSIS Pack <https://github.com/OpenNuvoton/cmsis-packs>`_:

- Download ``Nuvoton.NuMicroM33_DFP.x.y.z.pack`` where ``x.y.z`` is the most recent version.
- Under west workspace directory named ``zephyrproject`` or via ``west topdir``,
  create one pyOCD YAML file named ``pyocd.yaml``
  which has the content as follows:

  .. code-block:: yaml

     pack:
       - <path-to-Nuvoton.NuMicroM33_DFP.<x.y.z>.pack>

OTP system initialization is failed
===================================

Error message::

   [INF] Starting bootloader
   [ERR] OTP system initialization failed

OTP area can be corrupted by previous run of `TF-M`_ enabled application.
Try to re-initialize the OTP area for a fresh start.

The quick solution would be to do whole chip erase via e.g.
`NuMicro ICP Programming Tool <https://github.com/OpenNuvoton/Nuvoton_Tools>`_.

TrustZone disabled application gets to fail after `TF-M`_ enabled application was run before
============================================================================================

Partitioning of Flash and Data Flash will persist across reset cycle.
If users run `TF-M`_ enabled application ever and then get back to run
TrustZone disabled application on the same board hardware,
the latter will fail because it doesn't re-partition back.

The quick solution would be to do whole chip erase via e.g.
`NuMicro ICP Programming Tool <https://github.com/OpenNuvoton/Nuvoton_Tools>`_.


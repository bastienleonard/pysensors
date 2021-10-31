.. Copyright 2011, 2015, 2021 Bastien Léonard. All rights reserved.

.. Redistribution and use in source (reStructuredText) and 'compiled'
   forms (HTML, PDF, PostScript, RTF and so forth) with or without
   modification, are permitted provided that the following conditions are
   met:

.. 1. Redistributions of source code (reStructuredText) must retain
   the above copyright notice, this list of conditions and the
   following disclaimer as the first lines of this file unmodified.

.. 2. Redistributions in compiled form (converted to HTML, PDF,
   PostScript, RTF and other formats) must reproduce the above
   copyright notice, this list of conditions and the following
   disclaimer in the documentation and/or other materials provided
   with the distribution.

.. THIS DOCUMENTATION IS PROVIDED BY BASTIEN LÉONARD ``AS IS'' AND ANY
   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BASTIEN LÉONARD BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS DOCUMENTATION,
   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Dependencies
============

You will need to install the lm_sensors library first.

* Archlinux: install the `lm_sensors
  <https://www.archlinux.org/packages/?name=lm_sensors>`_ package.
* Debian/Ubuntu: install the libsensors4-dev package.
* Fedora: install the lm_sensors-devel and lm_sensors packages.

Feel free to give me the instructions for other distributions!


Building the module
===================

Use this command to create the module in the working directory::

   python setup.py build_ext --inplace

Without ``--inplace``, the module is placed in the build directory.

You can use ``python setup.py install`` to install the module in the
standard Python directory for libraries.

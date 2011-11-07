.. Copyright 2011 Bastien Léonard. All rights reserved.

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


Reference
=========

.. module:: sensors

Exceptions
----------

.. Exception:: SensorsException

   Raised when an error occurs. This normally means that a libsensors
   function call failed.

   .. attribute:: message

      The error message returned from the libsensors API.



Attributes
----------

.. attribute:: libsensors_version

   A string describing the libsensors version, e.g. ``'3.3.1'``.


Functions
---------

.. function:: cleanup

   You have to call this function when you don't need the module
   anymore. Use a ``try``/``finally`` block to make sure it's called::

      try:
          main()
      finally:
          sensors.cleanup()

.. function:: get_detected_chips([ChipName match])
.. function:: get_adapter_name(tuple (bus_type, bus_nr))


Classes
-------

.. class:: ChipName(prefix=None, bus_type=0, bus_nr=0, addr=0, path=None)

   .. attribute:: prefix
   .. attribute:: bus_type
   .. attribute:: bus_nr
   .. attribute:: addr
   .. attribute:: path

   .. method:: __repr__
   .. method:: __str__

      Return a user-friendly representation of the chip name, using
      ``sensors_snprintf_chip_name()``. Note that this C function will
      fail when “wildcards” are used, and __str__() will raise
      :exc:`SensorsException`. Wildcards are invalid values that have
      a special meaning, for example ``None`` can be used to match any
      chip name prefix.

   .. method:: get_features
   .. method:: get_all_subfeatures(feature)
   .. method:: get_label(feature)
   .. method:: get_value(int subfeat_nr)
   .. method:: set_value(int subfeat_nr, float value)
   .. method:: do_chip_sets
   .. staticmethod:: parse_chip_name(str orig_name)

.. class:: Feature(name=None, number=0, type=0)

   .. attribute:: name
   .. attribute:: number
   .. attribute:: type

   .. method:: __repr__

.. class:: SubFeature(name=None, number=0, type=0, mapping=0, flags=0)

   .. attribute:: name
   .. attribute:: number
   .. attribute:: type
   .. attribute:: mapping
   .. attribute:: flags

   .. method:: __repr__


Constants
---------

.. attribute:: API_VERSION
.. attribute:: BUS_NR_ANY
.. attribute:: BUS_NR_IGNORE
.. attribute:: BUS_TYPE_ACPI
.. attribute:: BUS_TYPE_ANY
.. attribute:: BUS_TYPE_HID
.. attribute:: BUS_TYPE_I2C
.. attribute:: BUS_TYPE_ISA
.. attribute:: BUS_TYPE_PCI
.. attribute:: BUS_TYPE_SPI
.. attribute:: BUS_TYPE_VIRTUAL
.. attribute:: CHIP_NAME_ADDR_ANY
.. attribute:: CHIP_NAME_PREFIX_ANY
.. attribute:: COMPUTE_MAPPING
.. attribute:: FEATURE_BEEP_ENABLE
.. attribute:: FEATURE_CURR
.. attribute:: FEATURE_ENERGY
.. attribute:: FEATURE_FAN
.. attribute:: FEATURE_HUMIDITY
.. attribute:: FEATURE_IN
.. attribute:: FEATURE_INTRUSION
.. attribute:: FEATURE_MAX_MAIN
.. attribute:: FEATURE_MAX_OTHER
.. attribute:: FEATURE_POWER
.. attribute:: FEATURE_TEMP
.. attribute:: FEATURE_UNKNOWN
.. attribute:: FEATURE_VID
.. attribute:: MODE_R
.. attribute:: MODE_W
.. attribute:: SUBFEATURE_BEEP_ENABLE
.. attribute:: SUBFEATURE_CURR_BEEP
.. attribute:: SUBFEATURE_CURR_CRIT
.. attribute:: SUBFEATURE_CURR_CRIT_ALARM
.. attribute:: SUBFEATURE_CURR_INPUT
.. attribute:: SUBFEATURE_CURR_LCRIT
.. attribute:: SUBFEATURE_CURR_LCRIT_ALARM
.. attribute:: SUBFEATURE_CURR_MAX
.. attribute:: SUBFEATURE_CURR_MAX_ALARM
.. attribute:: SUBFEATURE_CURR_MIN
.. attribute:: SUBFEATURE_CURR_MIN_ALARM
.. attribute:: SUBFEATURE_ENERGY_INPUT
.. attribute:: SUBFEATURE_FAN_BEEP
.. attribute:: SUBFEATURE_FAN_DIV
.. attribute:: SUBFEATURE_FAN_FAULT
.. attribute:: SUBFEATURE_FAN_INPUT
.. attribute:: SUBFEATURE_FAN_MIN
.. attribute:: SUBFEATURE_FAN_PULSES
.. attribute:: SUBFEATURE_HUMIDITY_INPUT
.. attribute:: SUBFEATURE_INTRUSION_ALARM
.. attribute:: SUBFEATURE_INTRUSION_BEEP
.. attribute:: SUBFEATURE_IN_BEEP
.. attribute:: SUBFEATURE_IN_CRIT
.. attribute:: SUBFEATURE_IN_CRIT_ALARM
.. attribute:: SUBFEATURE_IN_INPUT
.. attribute:: SUBFEATURE_IN_LCRIT
.. attribute:: SUBFEATURE_IN_LCRIT_ALARM
.. attribute:: SUBFEATURE_IN_MAX
.. attribute:: SUBFEATURE_IN_MAX_ALARM
.. attribute:: SUBFEATURE_IN_MIN
.. attribute:: SUBFEATURE_IN_MIN_ALARM
.. attribute:: SUBFEATURE_POWER_ALARM
.. attribute:: SUBFEATURE_POWER_AVERAGE
.. attribute:: SUBFEATURE_POWER_AVERAGE_HIGHEST
.. attribute:: SUBFEATURE_POWER_AVERAGE_LOWEST
.. attribute:: SUBFEATURE_POWER_CAP
.. attribute:: SUBFEATURE_POWER_CAP_ALARM
.. attribute:: SUBFEATURE_POWER_CAP_HYST
.. attribute:: SUBFEATURE_POWER_CRIT
.. attribute:: SUBFEATURE_POWER_CRIT_ALARM
.. attribute:: SUBFEATURE_POWER_INPUT
.. attribute:: SUBFEATURE_POWER_INPUT_HIGHEST
.. attribute:: SUBFEATURE_POWER_INPUT_LOWEST
.. attribute:: SUBFEATURE_POWER_MAX
.. attribute:: SUBFEATURE_POWER_MAX_ALARM
.. attribute:: SUBFEATURE_TEMP_BEEP
.. attribute:: SUBFEATURE_TEMP_CRIT
.. attribute:: SUBFEATURE_TEMP_CRIT_ALARM
.. attribute:: SUBFEATURE_TEMP_CRIT_HYST
.. attribute:: SUBFEATURE_TEMP_EMERGENCY
.. attribute:: SUBFEATURE_TEMP_EMERGENCY_ALARM
.. attribute:: SUBFEATURE_TEMP_EMERGENCY_HYST
.. attribute:: SUBFEATURE_TEMP_FAULT
.. attribute:: SUBFEATURE_TEMP_INPUT
.. attribute:: SUBFEATURE_TEMP_LCRIT
.. attribute:: SUBFEATURE_TEMP_LCRIT_ALARM
.. attribute:: SUBFEATURE_TEMP_MAX
.. attribute:: SUBFEATURE_TEMP_MAX_ALARM
.. attribute:: SUBFEATURE_TEMP_MAX_HYST
.. attribute:: SUBFEATURE_TEMP_MIN
.. attribute:: SUBFEATURE_TEMP_MIN_ALARM
.. attribute:: SUBFEATURE_TEMP_OFFSET
.. attribute:: SUBFEATURE_TEMP_TYPE
.. attribute:: SUBFEATURE_UNKNOWN
.. attribute:: SUBFEATURE_VID

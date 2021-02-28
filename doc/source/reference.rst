.. Copyright 2011, 2021 Bastien Léonard. All rights reserved.

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

   Return a list of :class:`ChipName` for all the detected chips
   matching the chip name. If *match* isn't provided, all the detected
   chips are returned.

.. function:: get_adapter_name(int bus_type, int bus_nr)

   Return the name of the bus, or ``None`` if it can't be found.

.. function:: init(filename)

   When you import ``sensors``, libsensors will load the configuration
   files from the default directory, which is what you want in most
   cases. Call this function if you need to load a different
   configuration file. Internally, it calls ``sensors_cleanup()`` and
   then ``sensors_init()`` with your file. If the file can't be
   opened, ``IOError`` is raised. If the initialization fails,
   :exc:`SensorsException` is raised.

.. function:: replace_parse_error_handler(handler)

   *handler* will be called when a parse error occurs. It will be
   passed three arguments: the error message, the filename and the
   line number. In some cases the filename may be ``None``, and the
   line number may be 0.

.. function:: replace_fatal_error_handler(handler)

   *handler* will be called when a fatal error occurs. It will be
   passed two arguments: the name of the function that failed, and the
   error message. If your function doesn't exit the process, it will
   be done automatically when after your function returns.


Classes
-------

.. class:: ChipName(prefix=None, bus_type=0, bus_nr=0, addr=0, path=None)

   .. describe:: repr(c)
   .. describe:: str(c)

      Return a user-friendly representation of the chip name, using
      ``sensors_snprintf_chip_name()``. Note that this C function will
      fail when “wildcards” are used, and __str__() will raise
      :exc:`SensorsException`. Wildcards are invalid values that have
      a special meaning, for example ``None`` can be used to match any
      chip name prefix.

   .. describe:: c1 == c2

      Return ``True`` if the members of ``c1`` and ``c2`` are equal.

   .. describe:: c1 != c2

      Equivalent to ``not c1 == c2``.

   .. attribute:: prefix
   .. attribute:: bus_type
   .. attribute:: bus_nr
   .. attribute:: addr
   .. attribute:: path

   .. method:: get_features
   .. method:: get_all_subfeatures(feature)
   .. method:: get_subfeature(feature, int type)

      Return the subfeature of *feature* that has *type*, or ``None``
      if it can't be found. *type* should be a constant such as
      :attr:`SUBFEATURE_TEMP_INPUT`, see :ref:`subfeatures-constants`.

   .. method:: get_label(feature)

      Return the label of the given feature. The chip shouldn't contain wilcard
      values.

   .. method:: get_value(int subfeat_nr)

      Return the value of a subfeature for the chip, as a
      ``float``. The chip shouldn't contain wildcard values.

   .. method:: get_value_or_none(int subfeat_nr)

      Return the value of a subfeature for the chip as a ``float``, or ``None``
      if an error occurred. The chip shouldn't contain wildcard values.

   .. method:: set_value(int subfeat_nr, float value)

      Set a value of the chip. The chip shouldn't contain wildcard
      values.

   .. method:: do_chip_sets

      Execute all set statements for the chip. The chip may contain
      contain wildcards.

   .. staticmethod:: parse_chip_name(str orig_name)

      Return a :class:`ChipName` object corresponding to the chip name
      represented by *orig_name*.


.. class:: Feature(name=None, number=0, type=0)

   You can think of features as categories for :class:`Subfeature` objects.

   .. describe:: repr(f)
   .. describe:: f1 == f2

      Return ``True`` if the members of ``f1`` and ``f2`` are equal.

   .. describe:: f1 != f2

      Equivalent to ``not f1 == f2``.

   .. attribute:: name
   .. attribute:: number
   .. attribute:: type

.. class:: Subfeature(name=None, number=0, type=0, mapping=0, flags=0)

   .. describe:: repr(s)
   .. describe:: s1 == s2

      Return ``True`` if the members of ``s1`` and ``s2`` are equal.

   .. describe:: s1 != s2

      Equivalent to ``not s1 == s2``.


   .. attribute:: name

      Used to refer to the feature in config files.

   .. attribute:: number

      Internal subfeature number, used throughout the API to refer to
      the subfeature.

   .. attribute:: type

      Subfeature type.

   .. attribute:: mapping

      Number of the main :class:`Feature` this subfeature belongs
      to. For example, subfeatures :attr:`SUBFEATURE_FAN_INPUT`,
      :attr:`SUBFEATURE_FAN_MIN`, :attr:`SUBFEATURE_FAN_DIV` and
      :attr:`SUBFEATURE_FAN_ALARM` are mapped to main feature
      :attr:`FEATURE_FAN`.

   .. attribute:: flags

      This is a bitfield, its value is a combination of :attr:`MODE_R`
      (readable), :attr:`MODE_W` (writable) and
      :attr:`COMPUTE_MAPPING` (affected by the computation rules of
      the main feature).


Constants
---------

API version
^^^^^^^^^^^

.. attribute:: LIBSENSORS_VERSION

   A string describing the libsensors version, e.g. ``'3.3.1'``.

.. attribute:: API_VERSION

   A number whose digits, in hexadecimal, represent the API
   version. The first digit is the major version (large changes that
   break the compatibility), the second one is for large changes, the
   third one is for small additions.

Bus numbers
^^^^^^^^^^^

.. attribute:: BUS_NR_ANY
.. attribute:: BUS_NR_IGNORE

Bus types
^^^^^^^^^

.. attribute:: BUS_TYPE_ACPI
.. attribute:: BUS_TYPE_ANY
.. attribute:: BUS_TYPE_HID
.. attribute:: BUS_TYPE_I2C
.. attribute:: BUS_TYPE_ISA
.. attribute:: BUS_TYPE_PCI
.. attribute:: BUS_TYPE_SPI
.. attribute:: BUS_TYPE_VIRTUAL

Chip names wilcards
^^^^^^^^^^^^^^^^^^^

.. attribute:: CHIP_NAME_ADDR_ANY
.. attribute:: CHIP_NAME_PREFIX_ANY

.. _features-constants:

Features
^^^^^^^^

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

Constants used in :attr:`Subfeature.flags`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. attribute:: MODE_R
.. attribute:: MODE_W
.. attribute:: COMPUTE_MAPPING

.. _subfeatures-constants:

Subfeatures
^^^^^^^^^^^
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
.. attribute:: SUBFEATURE_FAN_ALARM
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

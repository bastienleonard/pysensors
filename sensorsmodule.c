/*
 * Copyright 2011 Bastien Léonard. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY BASTIEN LÉONARD ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BASTIEN LÉONARD OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <Python.h>

#include <sensors/error.h>
#include <sensors/sensors.h>

#include "chipname.h"
#include "feature.h"
#include "subfeature.h"


static PyObject* cleanup(PyObject*, PyObject*);
static PyObject* get_detected_chips(PyObject*, PyObject*, PyObject*);
static PyObject* get_adapter_name(PyObject*, PyObject*, PyObject*);
static void add_constants(PyObject *module);
static PyObject* replace_parse_error_handler(PyObject*, PyObject*, PyObject*);
static void c_parse_error_handler(const char*, const char*, int);
static PyObject* replace_fatal_error_handler(PyObject*, PyObject*, PyObject*);
static void c_fatal_error_handler(const char*, const char*);

/* If someone ever compiles this on GCC < 4: you'll probably have to
 * remove the -fvisibility=hidden flags from the setup.py script to
 * build a working module.  Otherwise initsensors() won't be exported
 * in the shared object. */
#if __GNUC__ >= 4
__attribute__((__visibility__("default")))
#endif
PyMODINIT_FUNC initsensors(void);


PyObject *SensorsException = NULL;
static PyObject *py_libsensors_version = NULL;
static PyObject *py_parse_error_handler = NULL;
static PyObject *py_fatal_error_handler = NULL;

static PyMethodDef sensors_methods[] =
{
    {"cleanup", cleanup, METH_NOARGS, NULL},
    {"get_detected_chips", (PyCFunction)get_detected_chips, METH_KEYWORDS,
     NULL},
    {"get_adapter_name", (PyCFunction)get_adapter_name, METH_KEYWORDS, NULL},
    {"replace_parse_error_handler", (PyCFunction)replace_parse_error_handler,
     METH_KEYWORDS, NULL},
    {"replace_fatal_error_handler", (PyCFunction)replace_fatal_error_handler,
     METH_KEYWORDS, NULL},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC
initsensors(void)
{
    PyObject *module = NULL;

    ChipNameType.tp_new = PyType_GenericNew;
    FeatureType.tp_new = PyType_GenericNew;
    SubfeatureType.tp_new = PyType_GenericNew;

    if (PyType_Ready(&ChipNameType) < 0 ||
        PyType_Ready(&FeatureType) < 0 ||
        PyType_Ready(&SubfeatureType))
    {
        PyErr_SetString(PyExc_ImportError, "One or more PyType_Ready() failed");
        return;
    }

    module = Py_InitModule3(
        "sensors", sensors_methods,
        "Python binding for the lm_sensors API (libsensors)");

    if (module != NULL)
    {
        SensorsException = PyErr_NewExceptionWithDoc(
            "sensors.SensorsException",
            "Raised when an error occurs in the sensors module.",
            NULL, NULL);
        Py_INCREF(SensorsException);
        PyModule_AddObject(module, "SensorsException", SensorsException);

        Py_INCREF(&ChipNameType);
        PyModule_AddObject(module, "ChipName", (PyObject *)&ChipNameType);

        Py_INCREF(&FeatureType);
        PyModule_AddObject(module, "Feature", (PyObject*)&FeatureType);

        Py_INCREF(&FeatureType);
        PyModule_AddObject(module, "Subfeature", (PyObject*)&SubfeatureType);

        int status = sensors_init(NULL);

        /* TODO: document that the error can be thrown when importing
         * the module */
        if (status != 0)
        {
            PyErr_SetString(PyExc_ImportError, sensors_strerror(status));
            return;
        }

        py_libsensors_version = PyString_FromString(libsensors_version);
        PyModule_AddObject(module, "libsensors_version", py_libsensors_version);

        add_constants(module);
    }
}

static void add_constants(PyObject *module)
{
    PyObject *PY_SENSORS_API_VERSION = PyInt_FromLong(SENSORS_API_VERSION);
    PyModule_AddObject(module, "API_VERSION", PY_SENSORS_API_VERSION);

    PyObject *PY_SENSORS_CHIP_NAME_PREFIX_ANY = Py_None;
    Py_INCREF(Py_None);
    PyModule_AddObject(module, "CHIP_NAME_PREFIX_ANY",
                       PY_SENSORS_CHIP_NAME_PREFIX_ANY);

    PyObject *PY_SENSORS_CHIP_NAME_ADDR_ANY = PyInt_FromLong(
        SENSORS_CHIP_NAME_ADDR_ANY);
    PyModule_AddObject(module, "CHIP_NAME_ADDR_ANY",
                       PY_SENSORS_CHIP_NAME_ADDR_ANY);
    PyObject *PY_SENSORS_BUS_TYPE_ANY = PyInt_FromLong(SENSORS_BUS_TYPE_ANY);
    PyModule_AddObject(module, "BUS_TYPE_ANY", PY_SENSORS_BUS_TYPE_ANY);
    PyObject *PY_SENSORS_BUS_TYPE_I2C = PyInt_FromLong(SENSORS_BUS_TYPE_I2C);
    PyModule_AddObject(module, "BUS_TYPE_I2C", PY_SENSORS_BUS_TYPE_I2C);
    PyObject *PY_SENSORS_BUS_TYPE_ISA = PyInt_FromLong(SENSORS_BUS_TYPE_ISA);
    PyModule_AddObject(module, "BUS_TYPE_ISA", PY_SENSORS_BUS_TYPE_ISA);
    PyObject *PY_SENSORS_BUS_TYPE_PCI = PyInt_FromLong(SENSORS_BUS_TYPE_PCI);
    PyModule_AddObject(module, "BUS_TYPE_PCI", PY_SENSORS_BUS_TYPE_PCI);
    PyObject *PY_SENSORS_BUS_TYPE_SPI = PyInt_FromLong(SENSORS_BUS_TYPE_SPI);
    PyModule_AddObject(module, "BUS_TYPE_SPI", PY_SENSORS_BUS_TYPE_SPI);
    PyObject *PY_SENSORS_BUS_TYPE_VIRTUAL = PyInt_FromLong(
        SENSORS_BUS_TYPE_VIRTUAL);
    PyModule_AddObject(module, "BUS_TYPE_VIRTUAL", PY_SENSORS_BUS_TYPE_VIRTUAL);
    PyObject *PY_SENSORS_BUS_TYPE_ACPI = PyInt_FromLong(SENSORS_BUS_TYPE_ACPI);
    PyModule_AddObject(module, "BUS_TYPE_ACPI", PY_SENSORS_BUS_TYPE_ACPI);
    PyObject *PY_SENSORS_BUS_TYPE_HID = PyInt_FromLong(SENSORS_BUS_TYPE_HID);
    PyModule_AddObject(module, "BUS_TYPE_HID", PY_SENSORS_BUS_TYPE_HID);
    PyObject *PY_SENSORS_BUS_NR_ANY = PyInt_FromLong(SENSORS_BUS_NR_ANY);
    PyModule_AddObject(module, "BUS_NR_ANY", PY_SENSORS_BUS_NR_ANY);
    PyObject *PY_SENSORS_BUS_NR_IGNORE = PyInt_FromLong(SENSORS_BUS_NR_IGNORE);
    PyModule_AddObject(module, "BUS_NR_IGNORE", PY_SENSORS_BUS_NR_IGNORE);
    PyObject *PY_SENSORS_MODE_R = PyInt_FromLong(SENSORS_MODE_R);
    PyModule_AddObject(module, "MODE_R", PY_SENSORS_MODE_R);
    PyObject *PY_SENSORS_MODE_W = PyInt_FromLong(SENSORS_MODE_W);
    PyModule_AddObject(module, "MODE_W", PY_SENSORS_MODE_W);
    PyObject *PY_SENSORS_COMPUTE_MAPPING = PyInt_FromLong(
        SENSORS_COMPUTE_MAPPING);
    PyModule_AddObject(module, "COMPUTE_MAPPING", PY_SENSORS_COMPUTE_MAPPING);
    PyObject *PY_SENSORS_FEATURE_IN = PyInt_FromLong(SENSORS_FEATURE_IN);
    PyModule_AddObject(module, "FEATURE_IN", PY_SENSORS_FEATURE_IN);
    PyObject *PY_SENSORS_FEATURE_FAN = PyInt_FromLong(SENSORS_FEATURE_FAN);
    PyModule_AddObject(module, "FEATURE_FAN", PY_SENSORS_FEATURE_FAN);
    PyObject *PY_SENSORS_FEATURE_TEMP = PyInt_FromLong(SENSORS_FEATURE_TEMP);
    PyModule_AddObject(module, "FEATURE_TEMP", PY_SENSORS_FEATURE_TEMP);
    PyObject *PY_SENSORS_FEATURE_POWER = PyInt_FromLong(SENSORS_FEATURE_POWER);
    PyModule_AddObject(module, "FEATURE_POWER", PY_SENSORS_FEATURE_POWER);
    PyObject *PY_SENSORS_FEATURE_ENERGY = PyInt_FromLong(
        SENSORS_FEATURE_ENERGY);
    PyModule_AddObject(module, "FEATURE_ENERGY", PY_SENSORS_FEATURE_ENERGY);
    PyObject *PY_SENSORS_FEATURE_CURR = PyInt_FromLong(SENSORS_FEATURE_CURR);
    PyModule_AddObject(module, "FEATURE_CURR", PY_SENSORS_FEATURE_CURR);
    PyObject *PY_SENSORS_FEATURE_HUMIDITY = PyInt_FromLong(
        SENSORS_FEATURE_HUMIDITY);
    PyModule_AddObject(module, "FEATURE_HUMIDITY", PY_SENSORS_FEATURE_HUMIDITY);
    PyObject *PY_SENSORS_FEATURE_MAX_MAIN = PyInt_FromLong(
        SENSORS_FEATURE_MAX_MAIN);
    PyModule_AddObject(module, "FEATURE_MAX_MAIN", PY_SENSORS_FEATURE_MAX_MAIN);
    PyObject *PY_SENSORS_FEATURE_VID = PyInt_FromLong(SENSORS_FEATURE_VID);
    PyModule_AddObject(module, "FEATURE_VID", PY_SENSORS_FEATURE_VID);
    PyObject *PY_SENSORS_FEATURE_INTRUSION = PyInt_FromLong(
        SENSORS_FEATURE_INTRUSION);
    PyModule_AddObject(module, "FEATURE_INTRUSION",
                       PY_SENSORS_FEATURE_INTRUSION);
    PyObject *PY_SENSORS_FEATURE_MAX_OTHER = PyInt_FromLong(
        SENSORS_FEATURE_MAX_OTHER);
    PyModule_AddObject(module, "FEATURE_MAX_OTHER",
                       PY_SENSORS_FEATURE_MAX_OTHER);
    PyObject *PY_SENSORS_FEATURE_BEEP_ENABLE = PyInt_FromLong(
        SENSORS_FEATURE_BEEP_ENABLE);
    PyModule_AddObject(module, "FEATURE_BEEP_ENABLE",
                       PY_SENSORS_FEATURE_BEEP_ENABLE);
    PyObject *PY_SENSORS_FEATURE_UNKNOWN = PyInt_FromLong(
        SENSORS_FEATURE_UNKNOWN);
    PyModule_AddObject(module, "FEATURE_UNKNOWN", PY_SENSORS_FEATURE_UNKNOWN);
    PyObject *PY_SENSORS_SUBFEATURE_IN_INPUT = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_INPUT);
    PyModule_AddObject(module, "SUBFEATURE_IN_INPUT",
                       PY_SENSORS_SUBFEATURE_IN_INPUT);
    PyObject *PY_SENSORS_SUBFEATURE_IN_MIN = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_MIN);
    PyModule_AddObject(module, "SUBFEATURE_IN_MIN",
                       PY_SENSORS_SUBFEATURE_IN_MIN);
    PyObject *PY_SENSORS_SUBFEATURE_IN_MAX = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_MAX);
    PyModule_AddObject(module, "SUBFEATURE_IN_MAX",
                       PY_SENSORS_SUBFEATURE_IN_MAX);
    PyObject *PY_SENSORS_SUBFEATURE_IN_LCRIT = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_LCRIT);
    PyModule_AddObject(module, "SUBFEATURE_IN_LCRIT",
                       PY_SENSORS_SUBFEATURE_IN_LCRIT);
    PyObject *PY_SENSORS_SUBFEATURE_IN_CRIT = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_CRIT);
    PyModule_AddObject(module, "SUBFEATURE_IN_CRIT",
                       PY_SENSORS_SUBFEATURE_IN_CRIT);
    PyObject *PY_SENSORS_SUBFEATURE_IN_MIN_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_MIN_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_IN_MIN_ALARM",
                       PY_SENSORS_SUBFEATURE_IN_MIN_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_IN_MAX_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_MAX_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_IN_MAX_ALARM",
                       PY_SENSORS_SUBFEATURE_IN_MAX_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_IN_BEEP = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_BEEP);
    PyModule_AddObject(module, "SUBFEATURE_IN_BEEP",
                       PY_SENSORS_SUBFEATURE_IN_BEEP);
    PyObject *PY_SENSORS_SUBFEATURE_IN_LCRIT_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_LCRIT_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_IN_LCRIT_ALARM",
                       PY_SENSORS_SUBFEATURE_IN_LCRIT_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_IN_CRIT_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_IN_CRIT_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_IN_CRIT_ALARM",
                       PY_SENSORS_SUBFEATURE_IN_CRIT_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_FAN_INPUT = PyInt_FromLong(
        SENSORS_SUBFEATURE_FAN_INPUT);
    PyModule_AddObject(module, "SUBFEATURE_FAN_INPUT",
                       PY_SENSORS_SUBFEATURE_FAN_INPUT);
    PyObject *PY_SENSORS_SUBFEATURE_FAN_MIN = PyInt_FromLong(
        SENSORS_SUBFEATURE_FAN_MIN);
    PyModule_AddObject(module, "SUBFEATURE_FAN_MIN",
                       PY_SENSORS_SUBFEATURE_FAN_MIN);
    PyObject *PY_SENSORS_SUBFEATURE_FAN_FAULT = PyInt_FromLong(
        SENSORS_SUBFEATURE_FAN_FAULT);
    PyModule_AddObject(module, "SUBFEATURE_FAN_FAULT",
                       PY_SENSORS_SUBFEATURE_FAN_FAULT);
    PyObject *PY_SENSORS_SUBFEATURE_FAN_DIV = PyInt_FromLong(
        SENSORS_SUBFEATURE_FAN_DIV);
    PyModule_AddObject(module, "SUBFEATURE_FAN_DIV",
                       PY_SENSORS_SUBFEATURE_FAN_DIV);
    PyObject *PY_SENSORS_SUBFEATURE_FAN_BEEP = PyInt_FromLong(
        SENSORS_SUBFEATURE_FAN_BEEP);
    PyModule_AddObject(module, "SUBFEATURE_FAN_BEEP",
                       PY_SENSORS_SUBFEATURE_FAN_BEEP);
    PyObject *PY_SENSORS_SUBFEATURE_FAN_PULSES = PyInt_FromLong(
        SENSORS_SUBFEATURE_FAN_PULSES);
    PyModule_AddObject(module, "SUBFEATURE_FAN_PULSES",
                       PY_SENSORS_SUBFEATURE_FAN_PULSES);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_INPUT = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_INPUT);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_INPUT",
                       PY_SENSORS_SUBFEATURE_TEMP_INPUT);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_MAX = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_MAX);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_MAX",
                       PY_SENSORS_SUBFEATURE_TEMP_MAX);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_MAX_HYST = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_MAX_HYST);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_MAX_HYST",
                       PY_SENSORS_SUBFEATURE_TEMP_MAX_HYST);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_MIN = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_MIN);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_MIN",
                       PY_SENSORS_SUBFEATURE_TEMP_MIN);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_CRIT = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_CRIT);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_CRIT",
                       PY_SENSORS_SUBFEATURE_TEMP_CRIT);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_CRIT_HYST = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_CRIT_HYST);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_CRIT_HYST",
                       PY_SENSORS_SUBFEATURE_TEMP_CRIT_HYST);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_LCRIT = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_LCRIT);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_LCRIT",
                       PY_SENSORS_SUBFEATURE_TEMP_LCRIT);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_EMERGENCY = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_EMERGENCY);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_EMERGENCY",
                       PY_SENSORS_SUBFEATURE_TEMP_EMERGENCY);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_EMERGENCY_HYST",
                       PY_SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_MAX_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_MAX_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_MAX_ALARM",
                       PY_SENSORS_SUBFEATURE_TEMP_MAX_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_MIN_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_MIN_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_MIN_ALARM",
                       PY_SENSORS_SUBFEATURE_TEMP_MIN_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_CRIT_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_CRIT_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_CRIT_ALARM",
PY_SENSORS_SUBFEATURE_TEMP_CRIT_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_FAULT = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_FAULT);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_FAULT",
                       PY_SENSORS_SUBFEATURE_TEMP_FAULT);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_TYPE = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_TYPE);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_TYPE",
                       PY_SENSORS_SUBFEATURE_TEMP_TYPE);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_OFFSET = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_OFFSET);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_OFFSET",
                       PY_SENSORS_SUBFEATURE_TEMP_OFFSET);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_BEEP = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_BEEP);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_BEEP",
                       PY_SENSORS_SUBFEATURE_TEMP_BEEP);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_EMERGENCY_ALARM",
                       PY_SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_TEMP_LCRIT_ALARM",
                       PY_SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_AVERAGE = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_AVERAGE);
    PyModule_AddObject(module, "SUBFEATURE_POWER_AVERAGE",
                       PY_SENSORS_SUBFEATURE_POWER_AVERAGE);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST);
    PyModule_AddObject(module, "SUBFEATURE_POWER_AVERAGE_HIGHEST",
                       PY_SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST);
    PyModule_AddObject(module, "SUBFEATURE_POWER_AVERAGE_LOWEST",
                       PY_SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_INPUT = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_INPUT);
    PyModule_AddObject(module, "SUBFEATURE_POWER_INPUT",
                       PY_SENSORS_SUBFEATURE_POWER_INPUT);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST);
    PyModule_AddObject(module, "SUBFEATURE_POWER_INPUT_HIGHEST",
                       PY_SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_INPUT_LOWEST = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_INPUT_LOWEST);
    PyModule_AddObject(module, "SUBFEATURE_POWER_INPUT_LOWEST",
                       PY_SENSORS_SUBFEATURE_POWER_INPUT_LOWEST);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_CAP = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_CAP);
    PyModule_AddObject(module, "SUBFEATURE_POWER_CAP",
                       PY_SENSORS_SUBFEATURE_POWER_CAP);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_CAP_HYST = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_CAP_HYST);
    PyModule_AddObject(module, "SUBFEATURE_POWER_CAP_HYST",
                       PY_SENSORS_SUBFEATURE_POWER_CAP_HYST);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_MAX = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_MAX);
    PyModule_AddObject(module, "SUBFEATURE_POWER_MAX",
                       PY_SENSORS_SUBFEATURE_POWER_MAX);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_CRIT = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_CRIT);
    PyModule_AddObject(module, "SUBFEATURE_POWER_CRIT",
                       PY_SENSORS_SUBFEATURE_POWER_CRIT);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_POWER_ALARM",
                       PY_SENSORS_SUBFEATURE_POWER_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_CAP_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_CAP_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_POWER_CAP_ALARM",
                       PY_SENSORS_SUBFEATURE_POWER_CAP_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_MAX_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_MAX_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_POWER_MAX_ALARM",
                       PY_SENSORS_SUBFEATURE_POWER_MAX_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_POWER_CRIT_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_POWER_CRIT_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_POWER_CRIT_ALARM",
                       PY_SENSORS_SUBFEATURE_POWER_CRIT_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_ENERGY_INPUT = PyInt_FromLong(
        SENSORS_SUBFEATURE_ENERGY_INPUT);
    PyModule_AddObject(module, "SUBFEATURE_ENERGY_INPUT",
                       PY_SENSORS_SUBFEATURE_ENERGY_INPUT);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_INPUT = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_INPUT);
    PyModule_AddObject(module, "SUBFEATURE_CURR_INPUT",
                       PY_SENSORS_SUBFEATURE_CURR_INPUT);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_MIN = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_MIN);
    PyModule_AddObject(module, "SUBFEATURE_CURR_MIN",
                       PY_SENSORS_SUBFEATURE_CURR_MIN);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_MAX = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_MAX);
    PyModule_AddObject(module, "SUBFEATURE_CURR_MAX",
                       PY_SENSORS_SUBFEATURE_CURR_MAX);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_LCRIT = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_LCRIT);
    PyModule_AddObject(module, "SUBFEATURE_CURR_LCRIT",
                       PY_SENSORS_SUBFEATURE_CURR_LCRIT);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_CRIT = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_CRIT);
    PyModule_AddObject(module, "SUBFEATURE_CURR_CRIT",
                       PY_SENSORS_SUBFEATURE_CURR_CRIT);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_MIN_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_MIN_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_CURR_MIN_ALARM",
                       PY_SENSORS_SUBFEATURE_CURR_MIN_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_MAX_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_MAX_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_CURR_MAX_ALARM",
                       PY_SENSORS_SUBFEATURE_CURR_MAX_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_BEEP = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_BEEP);
    PyModule_AddObject(module, "SUBFEATURE_CURR_BEEP",
                       PY_SENSORS_SUBFEATURE_CURR_BEEP);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_LCRIT_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_LCRIT_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_CURR_LCRIT_ALARM",
                       PY_SENSORS_SUBFEATURE_CURR_LCRIT_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_CURR_CRIT_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_CURR_CRIT_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_CURR_CRIT_ALARM",
                       PY_SENSORS_SUBFEATURE_CURR_CRIT_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_HUMIDITY_INPUT = PyInt_FromLong(
        SENSORS_SUBFEATURE_HUMIDITY_INPUT);
    PyModule_AddObject(module, "SUBFEATURE_HUMIDITY_INPUT",
                       PY_SENSORS_SUBFEATURE_HUMIDITY_INPUT);
    PyObject *PY_SENSORS_SUBFEATURE_VID = PyInt_FromLong(
        SENSORS_SUBFEATURE_VID);
    PyModule_AddObject(module, "SUBFEATURE_VID", PY_SENSORS_SUBFEATURE_VID);
    PyObject *PY_SENSORS_SUBFEATURE_INTRUSION_ALARM = PyInt_FromLong(
        SENSORS_SUBFEATURE_INTRUSION_ALARM);
    PyModule_AddObject(module, "SUBFEATURE_INTRUSION_ALARM",
                       PY_SENSORS_SUBFEATURE_INTRUSION_ALARM);
    PyObject *PY_SENSORS_SUBFEATURE_INTRUSION_BEEP = PyInt_FromLong(
        SENSORS_SUBFEATURE_INTRUSION_BEEP);
    PyModule_AddObject(module, "SUBFEATURE_INTRUSION_BEEP",
                       PY_SENSORS_SUBFEATURE_INTRUSION_BEEP);
    PyObject *PY_SENSORS_SUBFEATURE_BEEP_ENABLE = PyInt_FromLong(
        SENSORS_SUBFEATURE_BEEP_ENABLE);
    PyModule_AddObject(module, "SUBFEATURE_BEEP_ENABLE",
                       PY_SENSORS_SUBFEATURE_BEEP_ENABLE);
    PyObject *PY_SENSORS_SUBFEATURE_UNKNOWN = PyInt_FromLong(
        SENSORS_SUBFEATURE_UNKNOWN);
    PyModule_AddObject(module, "SUBFEATURE_UNKNOWN",
                       PY_SENSORS_SUBFEATURE_UNKNOWN);
}

static PyObject*
cleanup(PyObject *self, PyObject *args)
{
    (void)self;
    (void)args;

    sensors_cleanup();

    Py_RETURN_NONE;
}

static PyObject*
get_detected_chips(PyObject *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"match", NULL};
    ChipName *match = NULL;
    PyObject *list = PyList_New(0);
    const sensors_chip_name *name = NULL;
    int n = 0;

    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O!", kwlist,
                                     &ChipNameType, &match))
    {
        goto error;
    }

    while (1)
    {
        name = sensors_get_detected_chips(
            match == NULL ? NULL : &match->chip_name,
            &n);

        if (name == NULL)
        {
            break;
        }
        else
        {
            PyObject *chip_name_args = Py_BuildValue(
                "shhis", name->prefix, name->bus.type, name->bus.nr, name->addr,
                name->path);

            if (chip_name_args == NULL)
            {
                goto error;
            }

            PyObject *py_name = PyObject_CallObject((PyObject*)&ChipNameType,
                                                    chip_name_args);
            Py_DECREF(chip_name_args);

            if (py_name == NULL)
            {
                goto error;
            }

            PyList_Append(list, py_name);
            Py_DECREF(py_name);
        }
    }

    return list;

error:
    Py_DECREF(list);
    return NULL;
}

static PyObject*
get_adapter_name(PyObject *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"bus", NULL};
    const sensors_bus_id bus = {-1, -1};

    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "(hh)", kwlist,
                                      &bus.type, &bus.nr))
    {
        return NULL;
    }

    const char *adapter_name = sensors_get_adapter_name(&bus);

    return PyString_FromString(adapter_name);
}

static PyObject*
replace_parse_error_handler(PyObject *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"handler", NULL};
    PyObject *func = NULL;

    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist,
                                     &func))
    {
        return NULL;
    }

    if (!PyCallable_Check(func))
    {
        PyErr_SetString(PyExc_TypeError,
                        "The handler argument must be callable");
        return NULL;
    }

    Py_XDECREF(py_parse_error_handler);
    py_parse_error_handler = func;
    Py_INCREF(py_parse_error_handler);
    sensors_parse_error_wfn = c_parse_error_handler;

    Py_RETURN_NONE;
}

static void c_parse_error_handler(const char *err, const char *filename,
                                  int lineno)
{
    if (py_parse_error_handler == NULL)
    {
        if (filename != NULL)
        {
            fprintf(stderr, "Parse error: ``%s'', in file ``%s'', at line %d\n"
                    "(The Python handler couldn't be called)\n", err, filename,
                    lineno);
        }
        else
        {
            fprintf(stderr, "Parse error: ``%s''\n"
                    "(The Python handler couldn't be called)\n", err);
        }
    }
    else
    {
        PyObject *args = Py_BuildValue("(ssi)", err, filename, lineno);
        PyObject_CallObject(py_parse_error_handler, args);
    }
}

static PyObject*
replace_fatal_error_handler(PyObject *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"handler", NULL};
    PyObject *func = NULL;

    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist,
                                     &func))
    {
        return NULL;
    }

    if (!PyCallable_Check(func))
    {
        PyErr_SetString(PyExc_TypeError,
                        "The handler argument must be callable");
        return NULL;
    }

    Py_XDECREF(py_fatal_error_handler);
    py_fatal_error_handler = func;
    Py_INCREF(py_fatal_error_handler);
    sensors_fatal_error = c_fatal_error_handler;

    Py_RETURN_NONE;
}

static void
c_fatal_error_handler(const char *proc, const char *err)
{
    if (py_fatal_error_handler == NULL)
    {
        fprintf(stderr, "Fatal error in `%s': %s\n"
                "(The Python handler couldn't be called)\n", proc, err);
        Py_Exit(EXIT_FAILURE);
    }
    else
    {
        PyObject *args = Py_BuildValue("(ss)", proc, err);
        PyObject_CallObject(py_fatal_error_handler, args);

        /* Exit the process, in case the user-defined handler didn't
         * do it already */
        Py_Exit(EXIT_FAILURE);
    }
}

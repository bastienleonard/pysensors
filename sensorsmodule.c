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


static PyObject * init(PyObject*, PyObject*, PyObject*);
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
    {"init", (PyCFunction)init, METH_KEYWORDS, NULL},
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
    PyModule_AddIntConstant(module, "API_VERSION", SENSORS_API_VERSION);

    PyObject *PY_SENSORS_CHIP_NAME_PREFIX_ANY = Py_None;
    Py_INCREF(Py_None);
    PyModule_AddObject(module, "CHIP_NAME_PREFIX_ANY",
                       PY_SENSORS_CHIP_NAME_PREFIX_ANY);

    PyModule_AddIntConstant(module, "CHIP_NAME_ADDR_ANY",
                            SENSORS_CHIP_NAME_ADDR_ANY);
    PyModule_AddIntConstant(module, "BUS_TYPE_ANY", SENSORS_BUS_TYPE_ANY);
    PyModule_AddIntConstant(module, "BUS_TYPE_I2C", SENSORS_BUS_TYPE_I2C);
    PyModule_AddIntConstant(module, "BUS_TYPE_ISA", SENSORS_BUS_TYPE_ISA);
    PyModule_AddIntConstant(module, "BUS_TYPE_PCI", SENSORS_BUS_TYPE_PCI);
    PyModule_AddIntConstant(module, "BUS_TYPE_SPI", SENSORS_BUS_TYPE_SPI);
    PyModule_AddIntConstant(module, "BUS_TYPE_VIRTUAL",
                            SENSORS_BUS_TYPE_VIRTUAL);
    PyModule_AddIntConstant(module, "BUS_TYPE_ACPI", SENSORS_BUS_TYPE_ACPI);
    PyModule_AddIntConstant(module, "BUS_TYPE_HID", SENSORS_BUS_TYPE_HID);
    PyModule_AddIntConstant(module, "BUS_NR_ANY", SENSORS_BUS_NR_ANY);
    PyModule_AddIntConstant(module, "BUS_NR_IGNORE", SENSORS_BUS_NR_IGNORE);
    PyModule_AddIntConstant(module, "MODE_R", SENSORS_MODE_R);
    PyModule_AddIntConstant(module, "MODE_W", SENSORS_MODE_W);
    PyModule_AddIntConstant(module, "COMPUTE_MAPPING", SENSORS_COMPUTE_MAPPING);
    PyModule_AddIntConstant(module, "FEATURE_IN", SENSORS_FEATURE_IN);
    PyModule_AddIntConstant(module, "FEATURE_FAN", SENSORS_FEATURE_FAN);
    PyModule_AddIntConstant(module, "FEATURE_TEMP", SENSORS_FEATURE_TEMP);
    PyModule_AddIntConstant(module, "FEATURE_POWER", SENSORS_FEATURE_POWER);
    PyModule_AddIntConstant(module, "FEATURE_ENERGY", SENSORS_FEATURE_ENERGY);
    PyModule_AddIntConstant(module, "FEATURE_CURR", SENSORS_FEATURE_CURR);
    PyModule_AddIntConstant(module, "FEATURE_HUMIDITY",
                            SENSORS_FEATURE_HUMIDITY);
    PyModule_AddIntConstant(module, "FEATURE_MAX_MAIN",
                            SENSORS_FEATURE_MAX_MAIN);
    PyModule_AddIntConstant(module, "FEATURE_VID", SENSORS_FEATURE_VID);
    PyModule_AddIntConstant(module, "FEATURE_INTRUSION",
                            SENSORS_FEATURE_INTRUSION);
    PyModule_AddIntConstant(module, "FEATURE_MAX_OTHER",
                            SENSORS_FEATURE_MAX_OTHER);
    PyModule_AddIntConstant(module, "FEATURE_BEEP_ENABLE",
                            SENSORS_FEATURE_BEEP_ENABLE);
    PyModule_AddIntConstant(module, "FEATURE_UNKNOWN", SENSORS_FEATURE_UNKNOWN);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_INPUT",
                            SENSORS_SUBFEATURE_IN_INPUT);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_MIN",
                            SENSORS_SUBFEATURE_IN_MIN);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_MAX",
                            SENSORS_SUBFEATURE_IN_MAX);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_LCRIT",
                            SENSORS_SUBFEATURE_IN_LCRIT);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_CRIT",
                            SENSORS_SUBFEATURE_IN_CRIT);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_MIN_ALARM",
                            SENSORS_SUBFEATURE_IN_MIN_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_MAX_ALARM",
                            SENSORS_SUBFEATURE_IN_MAX_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_BEEP",
                            SENSORS_SUBFEATURE_IN_BEEP);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_LCRIT_ALARM",
                            SENSORS_SUBFEATURE_IN_LCRIT_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_IN_CRIT_ALARM",
                            SENSORS_SUBFEATURE_IN_CRIT_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_FAN_INPUT",
                            SENSORS_SUBFEATURE_FAN_INPUT);
    PyModule_AddIntConstant(module, "SUBFEATURE_FAN_MIN",
                            SENSORS_SUBFEATURE_FAN_MIN);
    PyModule_AddIntConstant(module, "SUBFEATURE_FAN_FAULT",
                            SENSORS_SUBFEATURE_FAN_FAULT);
    PyModule_AddIntConstant(module, "SUBFEATURE_FAN_DIV",
                            SENSORS_SUBFEATURE_FAN_DIV);
    PyModule_AddIntConstant(module, "SUBFEATURE_FAN_BEEP",
                            SENSORS_SUBFEATURE_FAN_BEEP);
    PyModule_AddIntConstant(module, "SUBFEATURE_FAN_PULSES",
                            SENSORS_SUBFEATURE_FAN_PULSES);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_INPUT",
                            SENSORS_SUBFEATURE_TEMP_INPUT);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_MAX",
                            SENSORS_SUBFEATURE_TEMP_MAX);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_MAX_HYST",
                            SENSORS_SUBFEATURE_TEMP_MAX_HYST);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_MIN",
                            SENSORS_SUBFEATURE_TEMP_MIN);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_CRIT",
                            SENSORS_SUBFEATURE_TEMP_CRIT);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_CRIT_HYST",
                            SENSORS_SUBFEATURE_TEMP_CRIT_HYST);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_LCRIT",
                            SENSORS_SUBFEATURE_TEMP_LCRIT);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_EMERGENCY",
                            SENSORS_SUBFEATURE_TEMP_EMERGENCY);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_EMERGENCY_HYST",
                            SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_MAX_ALARM",
                            SENSORS_SUBFEATURE_TEMP_MAX_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_MIN_ALARM",
                            SENSORS_SUBFEATURE_TEMP_MIN_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_CRIT_ALARM",
                            SENSORS_SUBFEATURE_TEMP_CRIT_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_FAULT",
                            SENSORS_SUBFEATURE_TEMP_FAULT);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_TYPE",
                            SENSORS_SUBFEATURE_TEMP_TYPE);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_OFFSET",
                            SENSORS_SUBFEATURE_TEMP_OFFSET);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_BEEP",
                            SENSORS_SUBFEATURE_TEMP_BEEP);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_EMERGENCY_ALARM",
                            SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_TEMP_LCRIT_ALARM",
                            SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_AVERAGE",
                            SENSORS_SUBFEATURE_POWER_AVERAGE);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_AVERAGE_HIGHEST",
                            SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_AVERAGE_LOWEST",
                            SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_INPUT",
                            SENSORS_SUBFEATURE_POWER_INPUT);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_INPUT_HIGHEST",
                            SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_INPUT_LOWEST",
                            SENSORS_SUBFEATURE_POWER_INPUT_LOWEST);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_CAP",
                            SENSORS_SUBFEATURE_POWER_CAP);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_CAP_HYST",
                            SENSORS_SUBFEATURE_POWER_CAP_HYST);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_MAX",
                            SENSORS_SUBFEATURE_POWER_MAX);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_CRIT",
                            SENSORS_SUBFEATURE_POWER_CRIT);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_ALARM",
                            SENSORS_SUBFEATURE_POWER_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_CAP_ALARM",
                            SENSORS_SUBFEATURE_POWER_CAP_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_MAX_ALARM",
                            SENSORS_SUBFEATURE_POWER_MAX_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_POWER_CRIT_ALARM",
                            SENSORS_SUBFEATURE_POWER_CRIT_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_ENERGY_INPUT",
                            SENSORS_SUBFEATURE_ENERGY_INPUT);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_INPUT",
                            SENSORS_SUBFEATURE_CURR_INPUT);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_MIN",
                            SENSORS_SUBFEATURE_CURR_MIN);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_MAX",
                            SENSORS_SUBFEATURE_CURR_MAX);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_LCRIT",
                            SENSORS_SUBFEATURE_CURR_LCRIT);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_CRIT",
                            SENSORS_SUBFEATURE_CURR_CRIT);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_MIN_ALARM",
                            SENSORS_SUBFEATURE_CURR_MIN_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_MAX_ALARM",
                            SENSORS_SUBFEATURE_CURR_MAX_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_BEEP",
                            SENSORS_SUBFEATURE_CURR_BEEP);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_LCRIT_ALARM",
                            SENSORS_SUBFEATURE_CURR_LCRIT_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_CURR_CRIT_ALARM",
                            SENSORS_SUBFEATURE_CURR_CRIT_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_HUMIDITY_INPUT",
                            SENSORS_SUBFEATURE_HUMIDITY_INPUT);
    PyModule_AddIntConstant(module, "SUBFEATURE_VID", SENSORS_SUBFEATURE_VID);
    PyModule_AddIntConstant(module, "SUBFEATURE_INTRUSION_ALARM",
                            SENSORS_SUBFEATURE_INTRUSION_ALARM);
    PyModule_AddIntConstant(module, "SUBFEATURE_INTRUSION_BEEP",
                            SENSORS_SUBFEATURE_INTRUSION_BEEP);
    PyModule_AddIntConstant(module, "SUBFEATURE_BEEP_ENABLE",
                            SENSORS_SUBFEATURE_BEEP_ENABLE);
    PyModule_AddIntConstant(module, "SUBFEATURE_UNKNOWN",
                            SENSORS_SUBFEATURE_UNKNOWN);
}

static PyObject*
init(PyObject *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"filename", NULL};
    char *filename = NULL;

    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist, &filename))
    {
        return NULL;
    }

    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        PyErr_SetFromErrnoWithFilename(PyExc_IOError, filename);
        return NULL;
    }

    sensors_cleanup();
    int status = sensors_init(file);
    fclose(file);

    if (status != 0)
    {
        PyErr_SetString(SensorsException, sensors_strerror(status));
        return NULL;
    }

    Py_RETURN_NONE;
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
    char *kwlist[] = {"bus_type", "bus_nr", NULL};
    const sensors_bus_id bus = {-1, -1};

    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "hh", kwlist,
                                      &bus.type, &bus.nr))
    {
        return NULL;
    }

    const char *adapter_name = sensors_get_adapter_name(&bus);

    if (adapter_name == NULL)
    {
        Py_RETURN_NONE;
    }
    else
    {
        return PyString_FromString(adapter_name);
    }
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

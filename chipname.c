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
#include <structmember.h>

#include <sensors/sensors.h>
#include <sensors/error.h>

#include "sensorsmodule.h"
#include "chipname.h"
#include "feature.h"
#include "subfeature.h"

static int init(ChipName*, PyObject*, PyObject*);
static void dealloc(ChipName*);
static PyObject* repr(ChipName*);
static PyObject* str(ChipName*);
static PyObject* get_prefix(ChipName*, void*);
static int set_prefix(ChipName*, PyObject*, void*);
static PyObject* get_path(ChipName*, void*);
static int set_path(ChipName*, PyObject*, void*);
static PyObject* get_features(ChipName*, PyObject*);
static PyObject* get_all_subfeatures(ChipName*, PyObject*, PyObject*);
static PyObject* get_subfeature(ChipName*, PyObject*, PyObject*);
static PyObject* get_label(ChipName*, PyObject*, PyObject*);
static PyObject* get_value(ChipName*, PyObject*, PyObject*);
static PyObject* set_value(ChipName*, PyObject*, PyObject*);
static PyObject* do_chip_sets(ChipName*, PyObject*);
static PyObject* parse_chip_name(ChipName*, PyObject*, PyObject*);


static PyMethodDef methods[] = {
    {"get_features", (PyCFunction)get_features, METH_NOARGS,
     "Return all main features of a specific chip."},
    {"get_all_subfeatures", (PyCFunction)get_all_subfeatures, METH_KEYWORDS,
     NULL},
    {"get_subfeature", (PyCFunction)get_subfeature, METH_KEYWORDS, NULL},
    {"get_label", (PyCFunction)get_label, METH_KEYWORDS, NULL},
    {"get_value", (PyCFunction)get_value, METH_KEYWORDS, NULL},
    {"set_value", (PyCFunction)set_value, METH_KEYWORDS, NULL},
    {"do_chip_set", (PyCFunction)do_chip_sets, METH_NOARGS, NULL},
    {"parse_chip_name", (PyCFunction)parse_chip_name,
     METH_KEYWORDS | METH_STATIC, NULL},
    {NULL, NULL, 0, NULL}
};

static PyMemberDef members[] =
{
    {"bus_type", T_SHORT, offsetof(ChipName, chip_name.bus.type), 0, NULL},
    {"bus_nr", T_SHORT, offsetof(ChipName, chip_name.bus.nr), 0, NULL},
    {"addr", T_INT, offsetof(ChipName, chip_name.addr), 0, NULL},
    {NULL, 0, 0, 0, NULL}
};

static PyGetSetDef getsetters[] = {
    {"prefix", (getter)get_prefix, (setter)set_prefix, NULL, NULL},
    {"path", (getter)get_path, (setter)set_path, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject ChipNameType =
{
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "sensors.ChipName",        /*tp_name*/
    sizeof(ChipName),          /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)dealloc,       /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    (reprfunc)repr,            /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    (reprfunc)str,             /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "Contains the information encoded in a chip name.", /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    (PyMethodDef*)methods,     /* tp_methods */
    (PyMemberDef*)members,     /* tp_members */
    getsetters,                /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)init,            /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
};


static int
init(ChipName *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"prefix", "bus_type", "bus_nr", "addr", "path", NULL};
    const char* prefix = NULL;
    const char* path = NULL;
    int addr = 0;
    short bus_type = 0;
    short bus_nr = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|shhis", kwlist,
                                     &prefix, &bus_type, &bus_nr, &addr, &path))
    {
        return -1;
    }

    if (prefix != NULL)
    {
        self->chip_name.prefix = strdup(prefix);
    }

    self->chip_name.bus.type = bus_type;
    self->chip_name.bus.nr = bus_nr;
    self->chip_name.addr = addr;

    if (path != NULL)
    {
        self->chip_name.path = strdup(path);
    }

    return 0;
}

static void
dealloc(ChipName *self)
{
    free(self->chip_name.prefix);
    self->chip_name.prefix = NULL;
    free(self->chip_name.path);
    self->chip_name.path = NULL;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
repr(ChipName *self)
{
    const char *prefix = self->chip_name.prefix;
    const char *path = self->chip_name.path;

    if (prefix == NULL)
    {
        prefix = "None";
    }

    if (path == NULL)
    {
        path = "None";
    }

    return PyString_FromFormat("ChipName(prefix=%s, bus_type=%d, bus_nr=%d, "
                               "addr=%d, path=%s)",
                               prefix,
                               self->chip_name.bus.type, self->chip_name.bus.nr,
                               self->chip_name.addr, path);
}

static PyObject*
str(ChipName *self)
{
    char buffer[512];
    int status;

    status = sensors_snprintf_chip_name(buffer, sizeof buffer,
                                        &(self->chip_name));

    if (status < 0)
    {
        PyErr_SetString(SensorsException, sensors_strerror(status));
        return NULL;
    }

    return PyString_FromString(buffer);
}

static PyObject*
get_prefix(ChipName *self, void *closure)
{
    (void)closure;

    return PyString_FromString(self->chip_name.prefix);
}

static int
set_prefix(ChipName *self, PyObject *value, void *closure)
{
    (void)closure;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the prefix attribute");
        return -1;
    }

    if (!PyString_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, 
                        "The prefix attribute value must be a string");
        return -1;
    }

    free(self->chip_name.prefix);
    self->chip_name.prefix = strdup(PyString_AsString(value));

    return 0;
}

static PyObject*
get_path(ChipName *self, void *closure)
{
    (void)closure;
    return PyString_FromString(self->chip_name.prefix);
}

static int
set_path(ChipName *self, PyObject *value, void *closure)
{
    (void)closure;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the path attribute");
        return -1;
    }

    if (!PyString_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, 
                        "The path attribute value must be a string");
        return -1;
    }

    free(self->chip_name.path);
    self->chip_name.path = strdup(PyString_AsString(value));

    return 0;
}

static PyObject*
get_features(ChipName *self, PyObject *args)
{
    PyObject *list = PyList_New(0);
    int n = 0;

    (void)args;

    while (1)
    {
        const sensors_feature *feature = sensors_get_features(&self->chip_name,
                                                              &n);
        if (feature == NULL)
        {
            break;
        }
        else
        {
            /* Some internal fields are not accessible with
             * __init__(), so we need to copy them ourselves.  We
             * bypass __init__() altogether. */
            Feature *py_feature = PyObject_New(Feature,
                                               &FeatureType);

            if (py_feature == NULL)
            {
                goto error;
            }

            py_feature->feature = *feature;

            /* Duplicate the name string, so that destructors won't
             * end up freeing the same string pointer multiple
             * times. */
            py_feature->feature.name = strdup(feature->name);
            PyList_Append(list, (PyObject*)py_feature);
            Py_DECREF(py_feature);
        }
    }

    return list;

error:
    Py_DECREF(list);
    return NULL;
}

static PyObject*
get_all_subfeatures(ChipName *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"feature", NULL};
    Feature *feature = NULL;    

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist,
                                     &FeatureType, &feature))
    {
        return NULL;
    }

    PyObject *list = PyList_New(0);
    int n = 0;

    while (1)
    {
        const sensors_subfeature *subfeature = sensors_get_all_subfeatures(
            &self->chip_name, &feature->feature, &n);

        if (subfeature == NULL)
        {
            break;
        }
        else
        {
            PyObject *subfeature_args = Py_BuildValue(
                "siiiI",
                subfeature->name, subfeature->number, subfeature->type,
                subfeature->mapping, subfeature->flags);

            if (subfeature_args == NULL)
            {
                goto error;
            }

            PyObject *py_subfeature = PyObject_CallObject(
                (PyObject*)&SubfeatureType,
                subfeature_args);
            Py_DECREF(subfeature_args);

            if (py_subfeature == NULL)
            {
                goto error;
            }

            PyList_Append(list, py_subfeature);
            Py_DECREF(py_subfeature);
        }
    }

    return list;

error:
    Py_DECREF(list);
    return NULL;
}

static PyObject*
get_subfeature(ChipName *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"feature", "type", NULL};
    Feature *feature = NULL;
    int type = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!i", kwlist,
                                     &FeatureType, &feature, &type))
    {
        return NULL;
    }

    const sensors_subfeature *subfeature = sensors_get_subfeature(
        &self->chip_name, &feature->feature, type);

    if (subfeature == NULL)
    {
        Py_RETURN_NONE;
    }

    Subfeature *py_subfeature = PyObject_New(Subfeature, &SubfeatureType);

    if (py_subfeature == NULL)
    {
        return NULL;
    }

    py_subfeature->subfeature = *subfeature;
    py_subfeature->subfeature.name = strdup(subfeature->name);

    return (PyObject*)py_subfeature;
}

static PyObject*
get_label(ChipName *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"feature", NULL};
    Feature *feature = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!", kwlist,
                                     &FeatureType, &feature))
    {
        return NULL;
    }

    char *label = sensors_get_label(&self->chip_name, &feature->feature);

    if (label == NULL)
    {
        PyErr_SetString(SensorsException,
                        "sensors_get_label() returned NULL");
        return NULL;
    }

    PyObject *py_label = PyString_FromString(label);
    free(label);

    return py_label;
}

static PyObject*
get_value(ChipName *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"subfeat_nr", NULL};
    int subfeat_nr = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "i", kwlist, &subfeat_nr))
    {
        return NULL;
    }

    double value = 0.0;
    int status = sensors_get_value(&self->chip_name, subfeat_nr, &value);

    if (status < 0)
    {
        PyErr_SetString(SensorsException, sensors_strerror(status));
        return NULL;
    }

    return PyFloat_FromDouble(value);
}

static PyObject*
set_value(ChipName *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"subfeat_nr", "value", NULL};
    int subfeat_nr = -1;
    double value = 0.0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "id", kwlist,
                                     &subfeat_nr, &value))
    {
        return NULL;
    }

    int status = sensors_set_value(&self->chip_name, subfeat_nr, value);

    if (status < 0)
    {
        PyErr_SetString(SensorsException, sensors_strerror(status));
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject*
do_chip_sets(ChipName *self, PyObject *args)
{
    (void)args;

    int status = sensors_do_chip_sets(&self->chip_name);

    if (status < 0)
    {
        PyErr_SetString(SensorsException, sensors_strerror(status));
        return NULL;
    }

    Py_RETURN_NONE;
}

/*
 * This is a static method; self is NULL.
 */
static PyObject*
parse_chip_name(ChipName *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"orig_name", NULL};
    const char *orig_name = NULL;

    (void)self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist,
                                     &orig_name))
    {
        return NULL;
    }

    sensors_chip_name name = {NULL, {0, 0}, 0, NULL};
    int status = sensors_parse_chip_name(orig_name, &name);

    if (status < 0)
    {
        PyErr_SetString(SensorsException, sensors_strerror(status));
        return NULL;
    }

    ChipName *py_chip_name = PyObject_New(ChipName, &ChipNameType);

    if (py_chip_name == NULL)
    {
        return NULL;
    }

    /*
     * Copy the content of the obtained name into py_chip_name.  We
     * duplicate the strings, because name is supposed to freed by
     * sensors_free_chip_name(), instead of our own destructor.
     * Looking at the code of libsensors, it seems that it would
     * actually work just fine if our destructor cleaned it up, but I
     * prefer to be on the safe side.
     */
    py_chip_name->chip_name = name;

    if (py_chip_name->chip_name.prefix != NULL)
    {
        py_chip_name->chip_name.prefix = strdup(name.prefix);
    }

    if (py_chip_name->chip_name.path != NULL)
    {
        py_chip_name->chip_name.path = strdup(name.path);
    }

    sensors_free_chip_name(&name);

    return (PyObject*)py_chip_name;
}

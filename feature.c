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

#include <sensors/error.h>

#include "sensorsmodule.h"
#include "feature.h"


static int init(Feature*, PyObject*, PyObject*);
static void dealloc(Feature*);
static PyObject* repr(Feature*);
static PyObject* get_name(Feature*, void*);
static int set_name(Feature*, PyObject*, void*);


static PyMethodDef methods[] = {
    {NULL, NULL, 0, NULL} 
};

static PyMemberDef members[] =
{
    {"number", T_INT, offsetof(Feature, feature.number), 0, NULL},
    {"type", T_INT, offsetof(Feature, feature.type), 0, NULL},
    /* The two remaining members are not listed here because they are
     * for libsensors internal use only */
    {NULL, 0, 0, 0, NULL}
};

static PyGetSetDef getsetters[] = {
    {"name", (getter)get_name, (setter)set_name, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject FeatureType =
{
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "sensors.Feature",         /*tp_name*/
    sizeof(Feature),           /*tp_basicsize*/
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
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "Data about a single chip feature (or category leader).", /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    methods,                   /* tp_methods */
    members,                   /* tp_members */
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
init(Feature *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"name", "number", "type", NULL};
    const char *name = NULL;
    int number = 0;
    int type = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|sii", kwlist,
                                     &name, &number, &type))
    {
        return -1;
    }

    if (name != NULL)
    {
        self->feature.name = strdup(name);
    }

    self->feature.number = number;
    self->feature.type = type;
    self->feature.first_subfeature = 0;
    self->feature.padding1 = 0;    

    return 0;
}

static void
dealloc(Feature *self)
{
    free(self->feature.name);
    self->feature.name = NULL;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
repr(Feature *self)
{
    return PyString_FromFormat("Feature(name=%s, number=%d, type=%d)",
                               self->feature.name, self->feature.number,
                               self->feature.type);
}

static PyObject*
get_name(Feature *self, void *closure)
{
    (void)closure;

    return PyString_FromString(self->feature.name);
}

static int
set_name(Feature *self, PyObject *value, void *closure)
{
    (void)closure;

    if (value == NULL)
    {
        PyErr_SetString(PyExc_TypeError, "Cannot delete the name attribute");
        return -1;
    }

    if (!PyString_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, 
                        "The name attribute value must be a string");
        return -1;
    }

    free(self->feature.name);
    self->feature.name = strdup(PyString_AsString(value));

    return 0;
}

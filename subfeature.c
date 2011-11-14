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
#include "subfeature.h"


static int init(Subfeature*, PyObject*, PyObject*);
static void dealloc(Subfeature*);
static PyObject* repr(Subfeature*);
static PyObject* get_name(Subfeature*, void*);
static int set_name(Subfeature*, PyObject*, void*);


static PyMethodDef methods[] = {
    {NULL, NULL, 0, NULL} 
};

static PyMemberDef members[] =
{
    /* {"name", T_STRING, offsetof(Subfeature, subfeature.name), 0, NULL}, */
    {"number", T_INT, offsetof(Subfeature, subfeature.number), 0, NULL},
    {"type", T_INT, offsetof(Subfeature, subfeature.type), 0, NULL},
    {"mapping", T_INT, offsetof(Subfeature, subfeature.mapping), 0, NULL},
    {"flags", T_UINT, offsetof(Subfeature, subfeature.flags), 0, NULL},
    {NULL, 0, 0, 0, NULL}
};

static PyGetSetDef getsetters[] = {
    {"name", (getter)get_name, (setter)set_name, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject SubfeatureType =
{
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "sensors.Subfeature",         /*tp_name*/
    sizeof(Subfeature),           /*tp_basicsize*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    0,                          /* tp_doc */
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
init(Subfeature *self, PyObject *args, PyObject *kwargs)
{
    char *kwlist[] = {"name", "number", "type", "mapping", "flags",
                      NULL};
    const char *name = NULL;
    int number = 0;
    int type = 0;
    int mapping = 0;
    unsigned int flags = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|siiiI", kwlist,
                                     &name, &number, &type, &mapping, &flags))
    {
        return -1;
    }

    if (name != NULL)
    {
        self->subfeature.name = strdup(name);
    }

    self->subfeature.number = number;
    self->subfeature.type = type;
    self->subfeature.mapping = mapping;
    self->subfeature.flags = flags;

    return 0;
}

static void
dealloc(Subfeature *self)
{
    free(self->subfeature.name);
    self->subfeature.name = NULL;
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject*
repr(Subfeature *self)
{
    return PyString_FromFormat("SubFeature(name=%s, number=%d, type=%d, "
                               "mapping=%d, flags=%u)",
                               self->subfeature.name, self->subfeature.number,
                               self->subfeature.type, self->subfeature.mapping,
                               self->subfeature.flags);
}

static PyObject*
get_name(Subfeature *self, void *closure)
{
    (void)closure;

    return PyString_FromString(self->subfeature.name);
}

static int
set_name(Subfeature *self, PyObject *value, void *closure)
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

    free(self->subfeature.name);
    self->subfeature.name = strdup(PyString_AsString(value));

    return 0;
}

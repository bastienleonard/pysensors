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

#ifndef H_SENSORS_MODULE
#define H_SENSORS_MODULE

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

#ifndef IS_PY3K
#define INIT_TYPE_HEAD PyObject_HEAD_INIT(NULL) 0,
#else
#define INIT_TYPE_HEAD PyVarObject_HEAD_INIT(NULL, 0)
#endif

#ifndef IS_PY3K
#define FREE_OBJECT(o) o->ob_type->tp_free((PyObject*)o)
#else
#define FREE_OBJECT(o) Py_TYPE(o)->tp_free((PyObject*)o)
#endif

#ifdef IS_PY3K
#define PyString_FromString PyUnicode_FromString
#define PyString_FromFormat PyUnicode_FromFormat
#define PyString_Check PyUnicode_Check
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern PyObject *SensorsException;

#ifdef __cplusplus
}
#endif

#endif

#! /usr/bin/env python2
# -*- coding: utf-8 -*-

# Copyright 2011 Bastien Léonard. All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

#    1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.

#    2. Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.

# THIS SOFTWARE IS PROVIDED BY BASTIEN LÉONARD ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BASTIEN LÉONARD OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.


"""Script file used to generate boring stuff.

You don't need to use that code at all if you just want to build the
module, I'm just saving it here in case I need it later."""


import re


# I'm not including the code from sensors.h, since it's under LGPL.
# The idea is to generate code that adds the constants to the module,
# from C #define and enum declations.

code_defines1 = """"""

code_defines2 = """"""

code_enum1 = """"""

code_enum2 = """"""


def defines(code):
    matches = re.findall(r'^#define\s+(\w+)\s+[\d()-x]+$',
                         code, re.MULTILINE)
    return matches


def enum(code):
    matches = re.findall(r'^\s*(\w+)(?:\s+=\s*[\w\s<\d]+)?,$',
                         code, re.MULTILINE)
    return matches


def static_declarations():
    return ('\n'.join('static PyObject *PY_{0} = NULL;'.format(var)
                      if var != '\n' else ''
                      for var in defines(code_defines2) + ['\n'] +
                                 enum(code_enum1) +  ['\n'] + enum(code_enum2)))

def local_definitions():
    def python_name(c_name):
        return c_name.replace('SENSORS_', '')

    return ('\n'.join(('    PyObject *PY_{0} = PyInt_FromLong({0});\n'
                       '    PyModule_AddObject(module, "{1}", PY_{0});'
                       .format(c_name, python_name(c_name)))
                       for c_name in defines(code_defines1)))

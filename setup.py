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

from distutils.core import setup, Extension
import distutils.ccompiler


COMPILER_IS_GCC = True
DEBUG = False

extra_compiler_args = []
extra_linker_args = []

if COMPILER_IS_GCC:
    extra_compiler_args.extend(('-Wall -Wextra -Wno-missing-field-initializers '
                                '-pedantic -std=c99 -fvisibility=hidden')
                                .split())
    extra_linker_args.extend('-fvisibility=hidden'.split())

    if DEBUG:
        extra_compiler_args.extend('-g -ggdb'.split())

module = Extension('sensors',
                   sources = ['sensorsmodule.c', 'chipname.c', 'feature.c',
                              'subfeature.c'],
                   libraries=['sensors'],
                   extra_compile_args=extra_compiler_args,
                   extra_link_args=extra_linker_args)

setup(name = 'sensors',
      version = '0.1',
      description = 'Python binding for libsensors',
      ext_modules = [module])

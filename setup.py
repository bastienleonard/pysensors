# -*- coding: utf-8 -*-

# Copyright 2011, 2021 Bastien Léonard. All rights reserved.

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

import glob

from distutils.core import setup, Extension
import distutils.ccompiler


EXTRA_COMPILE_ARGS = [
    '-Wall',
    '-Wextra',
    '-Wno-missing-field-initializers',
    '-pedantic',
    '-std=c99',
    '-fvisibility=hidden'
]
EXTRA_LINK_ARGS = ['-fvisibility=hidden']


with open('README.md') as f:
    long_description = f.read()

setup(
    name='sensors',
    version='0.1',
    description='Python binding for lm_sensors (Linux monitoring sensors)',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/bastienleonard/pysensors',
    author='Bastien Léonard',
    author_email='bastien.leonard@gmail.com',
    license='BSD',
    platforms=[
        'Linux'
    ],
    classifiers=[
        'License :: OSI Approved :: BSD License',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Topic :: System :: Hardware',
        'Topic :: System :: Monitoring'
    ],
    ext_modules=[
        Extension(
            'sensors',
            sources=glob.glob('src/*.c'),
            libraries=['sensors'],
            extra_compile_args=EXTRA_COMPILE_ARGS,
            extra_link_args=EXTRA_LINK_ARGS
        )
    ]
)

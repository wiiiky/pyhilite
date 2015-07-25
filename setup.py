#!/usr/bin/env python3
# -*- coding: utf-8 -*-

try:
    from setuptools import setup, find_packages, Extension
except:
    from distutils.core import setup, find_packages, Extension


srchilite = Extension('pyhilite.srchilite',
                      libraries=['source-highlight'],
                      sources=['pyhilite/srchilite.cc'])

setup(
    name='pyhilite',
    version='0.2.0',
    description='Python3 binding of GNU source-highlight C++ library',
    long_description=''.join(open('README').readlines()),
    keywords='Source Code, Highlight, HTML, Latex',
    author='Wiky L',
    author_email='wiiiky@outlook.com',
    license='GPLv3',
    scripts=['bin/highlight.py', 'bin/hicat.py'],
    packages=['pyhilite'],
    package_dir={
        'pyhilite': 'pyhilite',
    },
    package_data={
        'pyhilite': ['data/*']
    },
    ext_modules=[srchilite],
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU Lesser General Public License v3 or later (LGPLv3+)',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Python',
    ],
    test_suite = 'tests.lib.PyHiLiteTests'
)

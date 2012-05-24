#!/usr/bin/env python

from distutils.core import setup

setup(  name='NeMo',
        description='Spiking neural network simulator',
        long_description=''''
NeMo supports fast simulation of Izhikevich neurons on CUDA-enabled GPUs. The
Python module is a siple wrapper for the underlying C++ class library, which
must be installed for the simulator to work.
''',
        version='0.7.2',
        author='Andreas Fidjeland',
        author_email='andreas.fidjeland@imperial.ac.uk',
        url='http://sourceforge.net/projects/nemosim',
        download_url='http://sourceforge.net/projects/nemosim/files/0.7.2',
        platforms=['Linux', 'Windows', 'OSX'],
        license='GPLv2',
        packages=['nemo'],
        # There is an extension module here, but the shared library itself is
        # stored in a NeMo-specific directory to which the __init__ script sets
        # the path.
    )

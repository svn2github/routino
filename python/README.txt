                                 ROUTINO PYTHON
                                 ==============

This directory contains a Python version 3 interface to the Routino routing
database that allows routes to be calculated.  The same functionality as the
Routino library (libroutino) is provided because it is implemented as a wrapper
around that library.

Compilation
-----------

A working Python 3 installation and the Swig tool are required to be able to
compile this Python module, run 'make'.

Installation
------------

To install the Python module run 'python setup.py install'.

Running
-------

Test scripts are included in the 'test' directory, run 'make test' to run them.
The tests verify that the results of the Python version are identical to the
results of the compiled version.

To run the test scripts the path to the compiled Python module and the
libroutino library are set up automatically, no installation is required.

To use the Python module normally it must be installed and the libroutino
library must also be installed in a directory that is searched for libraries.
The Python example router 'router.py' accepts the same command line arguments as
the compiled versions.

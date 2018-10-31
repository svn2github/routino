                                 ROUTINO PYTHON
                                 ==============

This directory contains a Python version 3 interface to the Routino routing
database that allows routes to be calculated.  The same functionality as the
Routino library (libroutino) is provided because it is implemented as a wrapper
around that library.

Compilation
-----------

To compile the Python module run 'make'.  A working Python 3 installation and
the Swig tool are required to be able to compile this Python module.  If they
are not available then a warning will be printed but no error occur.

Running 'make' in the top level directory will also try to build the module.

Testing
-------

To run the test scripts run 'make test'.  The tests verify that the results of
the Python version are identical to the results of the compiled version.

Running 'make test' in the top level directory will also try to run the tests
for the Python module.

Installation
------------

To install the Python module run 'make install'.  The installation directory is
the one defined in 'Makefile.conf'.

Running 'make install' in the top level directory will also try to install the
module.

Using
-----

To use the Python module normally it must be installed and the libroutino
library must also be installed in a directory that is searched for libraries.
The Python example router 'router.py' accepts the same command line arguments as
the compiled versions.

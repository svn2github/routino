# Python interface setup script
#
# Part of the Routino routing software.
#
# This file Copyright 2018 Andrew M. Bishop
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import os
import re
from distutils.core import setup, Extension

routino_router = Extension('routino._router',
                           sources = ['src/_router.c'],
                           include_dirs = ['../src'],
                           library_dirs = ['../src'],
                           libraries = ['routino'])

setup (name = 'Routino',
       version = '1.0',
       author="Andrew M. Bishop", author_email='amb@routino.org',
       url='http://routino.org/',
       description = 'Interfaces to Routino in Python',
       packages = ['routino'],
       package_dir = {'routino': 'src'},
       py_modules = ['routino', 'routino.router'],
       ext_modules = [routino_router])

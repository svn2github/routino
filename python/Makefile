# Python interface Makefile
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

# All configuration is in the top-level Makefile.conf

include ../Makefile.conf

# Programs

SWIG=swig

PYTHON=python3

# Compilation targets

PY_FILES=$(wildcard src/*.py)

C_FILES=$(wildcard src/*.c)

SWIG_C=src/_router.c

BUILD_TIMESTAMP=build/.timestamp

########

all: $(BUILD_TIMESTAMP)

########

$(BUILD_TIMESTAMP): $(SWIG_C) $(PY_FILES) $(C_FILES) setup.py
	@rm -f $@
	$(PYTHON) setup.py build && touch $(BUILD_TIMESTAMP)

src/_router.c : src/router.i ../src/routino.h
	$(SWIG) -python -o $@ $<

src/%.o : src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

########

test: $(BUILD_TIMESTAMP)
	cd test && $(MAKE) $@

########

install: all

########

clean:
	rm -f *~

########

distclean:
	rm -rf build

########

.PHONY:: all test install clean distclean

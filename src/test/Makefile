# Test cases Makefile
#
# Part of the Routino routing software.
#
# This file Copyright 2011-2015, 2018 Andrew M. Bishop
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

include ../../Makefile.conf

# Executables

EXE=is-fast-math$(.EXE)

########

all :

########

test : test-exe $(EXE)
	@./run-all-tests.sh

########

test-exe :
	cd .. && $(MAKE) all-exe all-lib

is-fast-math$(.EXE) : is-fast-math.o
	$(LD) $< -o $@ $(LDFLAGS)

is-fast-math.o : is-fast-math.c
	$(CC) -c $(CFLAGS) $< -o $@

########

install:

########

clean:
	rm -rf fat
	rm -rf slim
	rm -rf fat+lib
	rm -rf slim+lib
	rm -rf fat-pruned
	rm -rf slim-pruned
	rm -f *.log
	rm -f *~
	rm -f *.o
	rm -f $(EXE)
	rm -f core
	rm -f *.gcda *.gcno *.gcov gmon.out

########

distclean: clean

########

.PHONY:: all test install clean distclean

.PHONY:: test-exe

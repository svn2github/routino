# XML test programs Makefile
#
# Part of the Routino routing software.
#
# This file Copyright 2010-2015, 2017 Andrew M. Bishop
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

# Compilation targets

DEPDIR=.deps

XMLDIR=../../xml

X=$(notdir $(wildcard $(XMLDIR)/*.xsd))
C=$(foreach f,$(X),$(addsuffix -skeleton.c,$(basename $f)))
D=$(wildcard $(DEPDIR)/*.d)
O=$(foreach f,$(C),$(addsuffix .o,$(basename $f)))
E=$(foreach f,$(C),$(addsuffix $(.EXE),$(basename $f)))

EXE=xsd-to-xmlparser$(.EXE)

########

all: $(EXE) $(C) $(E)

########

xsd-to-xmlparser$(.EXE) : xsd-to-xmlparser.o ../xmlparse.o
	$(LD) $^ -o $@ $(LDFLAGS)

########

%-skeleton.c : $(XMLDIR)/%.xsd xsd-to-xmlparser$(.EXE)
	-./xsd-to-xmlparser < $< > $@
	@test -s $@ || rm $@

%-skeleton$(.EXE) : %-skeleton.o ../xmlparse.o
	$(LD) $^ -o $@ $(LDFLAGS)

.SECONDARY : $(O)

########

../xmlparse.o : ../xmlparse.c ../xmlparse.h
	cd .. && $(MAKE) xmlparse.o

########

%.o : %.c
	-@[ -d $(DEPDIR) ] || mkdir $(DEPDIR) || true
	$(CC) -c $(CFLAGS) -I.. $< -o $@ -MMD -MP -MF $(addprefix $(DEPDIR)/,$(addsuffix .d,$(basename $@)))

########

test: test-skeleton$(.EXE)
	@status=true ;\
	echo "" ;\
	for good in test/good*.xml; do \
	   echo "Testing: $$good ... " ;\
	   if ./test-skeleton < $$good > /dev/null; then echo "... passed"; else echo "... FAILED"; status=false; fi ;\
	   echo "" ;\
	done ;\
	for bad in test/bad*.xml; do \
	   echo "Testing: $$bad ... " ;\
	   if ./test-skeleton < $$bad > /dev/null; then echo "... FAILED"; status=false; else echo "... passed"; fi ;\
	   echo "" ;\
	done ;\
	if $$status; then echo "Success: all tests passed"; else echo "Warning: Some tests FAILED"; fi ;\
	$$status

test-skeleton$(.EXE) : test-skeleton.o ../xmlparse.o
	$(LD) $^ -o $@ $(LDFLAGS)

test-skeleton.c : test/test.xsd xsd-to-xmlparser$(.EXE)
	./xsd-to-xmlparser < $< | sed -e 's/XMLPARSE_UNKNOWN_ATTR_WARN/XMLPARSE_UNKNOWN_ATTR_ERROR/' > $@

########

install:

########

clean:
	rm -f *~
	rm -f *.o
	rm -f $(EXE)
	rm -f $(E) test-skeleton$(.EXE)
	rm -f $(D) $(DEPDIR)/test-skeleton.d
	rm -f $(C) test-skeleton.c
	rm -fr $(DEPDIR)
	rm -f core
	rm -f *.gcda *.gcno *.gcov gmon.out

########

distclean: clean

########

include $(D)

########

.PHONY:: all test install clean distclean

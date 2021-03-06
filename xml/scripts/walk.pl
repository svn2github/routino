#!/usr/bin/perl
#
# Special case tagging rule generator.
#
# Part of the Routino routing software.
#
# This file Copyright 2011-2014 Andrew M. Bishop
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

use strict;

# Process the input

while(<STDIN>)
  {
   if(m%</way>%)
     {
      print "    <!-- Special case for walking -->\n";
      print "\n";
      print "    <if>\n";
      print "      <output k=\"horse\"      v=\"no\"/>\n";
      print "      <output k=\"bicycle\"    v=\"no\"/>\n";
      print "      <output k=\"moped\"      v=\"no\"/>\n";
      print "      <output k=\"motorcycle\" v=\"no\"/>\n";
      print "      <output k=\"motorcar\"   v=\"no\"/>\n";
      print "      <output k=\"goods\"      v=\"no\"/>\n";
      print "      <output k=\"hgv\"        v=\"no\"/>\n";
      print "      <output k=\"psv\"        v=\"no\"/>\n";
      print "\n";
      print "      <output k=\"oneway\" v=\"no\"/>\n";
      print "\n";
      print "      <output k=\"bridge\" v=\"no\"/>\n";
      print "      <output k=\"tunnel\" v=\"no\"/>\n";
      print "\n";
      print "      <output k=\"bicycleroute\"  v=\"no\"/>\n";
      print "      <output k=\"cyclebothways\" v=\"no\"/>\n";
      print "    </if>\n";
      print "\n";
     }

   if(m%</relation>%)
     {
      print "    <!-- Special case for walking -->\n";
      print "\n";
      print "    <if>\n";
      print "      <output k=\"restriction\" v=\"no\"/>\n";
      print "\n";
      print "      <output k=\"bicycleroute\" v=\"no\"/>\n";
      print "    </if>\n";
      print "\n";
     }

   print;
  }

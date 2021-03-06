#!/usr/bin/perl
#
# Routino router results retrieval CGI
#
# Part of the Routino routing software.
#
# This file Copyright 2008-2014, 2016 Andrew M. Bishop
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

# Use the generic router script
require "./router.pl";

# Use the perl CGI module
use CGI ':cgi';


# Create the query and get the parameters

my $query=new CGI;

my @rawparams=$query->param;

# Legal CGI parameters with regexp validity check

my %legalparams=(
              "type"   => "(shortest|quickest|router)",
              "format" => "(html|gpx-route|gpx-track|text|text-all|log)",

              "uuid"   => "[0-9a-f]{32}"
             );

# Validate the CGI parameters, ignore invalid ones

my %cgiparams=();

foreach my $key (@rawparams)
  {
   foreach my $test (keys (%legalparams))
     {
      if($key =~ m%^$test$%)
        {
         my $value=$query->param($key);

         if($value =~ m%^$legalparams{$test}$%)
           {
            $cgiparams{$key}=$value;
            last;
           }
        }
     }
  }

# Parse the parameters

my $uuid  =$cgiparams{"uuid"};
my $type  =$cgiparams{"type"};
my $format=$cgiparams{"format"};

# Return the file

ReturnOutput($uuid,$type,$format);

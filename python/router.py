#!/usr/bin/python3
##########################################
# OSM router calling libroutino library from Python.
#
# Part of the Routino routing software.
##########################################
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
##########################################

import argparse
import sys
import os
import math

import routino.router as routino


# Parse the command line arguments

argparser = argparse.ArgumentParser(description="Calculates a route using Routino and command line data.")

argparser.add_argument("--version",          dest="version",       action='store_false',   help="Print the version of Routino.")

argparser.add_argument("--dir",              dest="dirname",       type=str, default=None, help="The directory containing the routing database.")
argparser.add_argument("--prefix",           dest="prefix",        type=str, default=None, help="The filename prefix for the routing database.")

argparser.add_argument("--profiles",         dest="profiles",      type=str, default=None, help="The name of the XML file containing the profiles (defaults to 'profiles.xml' with '--dir' and '--prefix' options).")
argparser.add_argument("--translations",     dest="translations",  type=str, default=None, help="The name of the XML file containing the translations (defaults to 'translations.xml' with '--dir' and '--prefix' options).")

argparser.add_argument("--reverse",          dest="reverse",       action='store_true',    help="Find a route between the waypoints in reverse order.")
argparser.add_argument("--loop",             dest="loop",          action='store_true',    help="Find a route that returns to the first waypoint.")

argparser.add_argument("--output-html",      dest="html",          action='store_true',    help="Write an HTML description of the route.")
argparser.add_argument("--output-gpx-track", dest="gpx_track",     action='store_true',    help="Write a GPX track file with all route points.")
argparser.add_argument("--output-gpx-route", dest="gpx_route",     action='store_true',    help="Write a GPX route file with interesting junctions.")
argparser.add_argument("--output-text",      dest="text",          action='store_true',    help="Write a plain text file with interesting junctions.")
argparser.add_argument("--output-text-all",  dest="text_all",      action='store_true',    help="Write a plain text file with all route points.")
argparser.add_argument("--output-none",      dest="none",          action='store_true',    help="Don't write any output files or read any translations. (If no output option is given then all are written.)")
argparser.add_argument("--output-stdout",    dest="use_stdout",    action='store_true',    help="Write to stdout instead of a file (requires exactly one output format option, implies '--quiet').")

argparser.add_argument("--list-html",        dest="list_html",     action='store_true',    help="Create an HTML list of the route.")
argparser.add_argument("--list-html-all",    dest="list_html_all", action='store_true',    help="Create an HTML list of the route with all points.")
argparser.add_argument("--list-text",        dest="list_text",     action='store_true',    help="Create a plain text list with interesting junctions.")
argparser.add_argument("--list-text-all",    dest="list_text_all", action='store_true',    help="Create a plain text list with all route points.")

argparser.add_argument("--profile",          dest="profilename",   type=str, default=None, help="Select the loaded profile with this name.")
argparser.add_argument("--language",         dest="language",      type=str, default=None, help="Use the translations for specified language.")

argparser.add_argument("--quickest",         dest="shortest",      action='store_false',   help="Find the quickest route between the waypoints.")
argparser.add_argument("--shortest",         dest="shortest",      action='store_true',    help="Find the shortest route between the waypoints.")

argparser.add_argument("--lon",              dest="lons",          action='append', type=float, help="Specify the longitude of the next waypoint (can also use '--lon<n>' to specify the n'th longitude).")
argparser.add_argument("--lat",              dest="lats",          action='append', type=float, help="Specify the latitude of the next waypoint (can also use '--lat<n>' to specify the n'th latitude).")

for i in range(1,99):
    argparser.add_argument("--lon"+str(i),   dest="lon"+str(i),    type=float, help=argparse.SUPPRESS)
    argparser.add_argument("--lat"+str(i),   dest="lat"+str(i),    type=float, help=argparse.SUPPRESS)

args = argparser.parse_args()


# Check the specified command line options

if args.use_stdout and (int(args.html)+int(args.gpx_track)+int(args.gpx_route)+int(args.text)+int(args.text_all))!=1:
    print("Error: The '--output-stdout' option requires exactly one other output option (but not '--output-none').")
    sys.exit(1)

if not args.html and not args.gpx_track and not args.gpx_route and not args.text and not args.text_all and not args.none:
    args.html=True
    args.gpx_track=True
    args.gpx_route=True
    args.text=True
    args.text_all=True


# Load in the selected profiles

if args.profiles is not None:
    if not os.access(args.profiles,os.F_OK):
        print("Error: The '--profiles' option specifies a file '{:s}' that does not exist.".format(args.profiles))
        sys.exit(1)
else:
    args.profiles=routino.FileName(args.dirname,args.prefix,"profiles.xml")

    if not os.access(args.profiles,os.F_OK):
        defaultprofiles = routino.FileName("../xml/","routino","profiles.xml")

        if not os.access(defaultprofiles,os.F_OK):
            print("Error: The '--profiles' option was not used and the files '{:s}' and '{:s}' do not exist.".format(args.profiles,defaultprofiles))
            sys.exit(1)

        args.profiles=defaultprofiles

if args.profilename is None:
    print("Error: A profile name must be specified")
    sys.exit(1)

if routino.ParseXMLProfiles(args.profiles):
    print("Error: Cannot read the profiles in the file '{:s}'.".format(args.profiles))
    sys.exit(1)

profile=routino.GetProfile(args.profilename)

if profile is None:
    list = routino.GetProfileNames()

    print("Error: Cannot find a profile called '{:s}' in the file '{:s}'.".format(args.profilename,args.profiles))
    print("Profiles available are: {:s}.".format(", ".join(list)))
    sys.exit(1)


# Load in the selected translation

if args.translations is not None:
    if not os.access(args.translations,os.F_OK):
        print("Error: The '--translations' option specifies a file '{:s}' that does not exist.".format(args.translations))
        sys.exit(1)

else:
    args.translations=routino.FileName(args.dirname,args.prefix,"translations.xml")

    if not os.access(translations,os.F_OK):
        defaulttranslations = routino.FileName("../xml/","routino","translations.xml")

        if not os.access(defaulttranslations,os.F_OK):
            print("Error: The '--translations' option was not used and the files '{:s}' and '{:s}' do not exist.".format(args.translations,defaulttranslations))
            sys.exit(1)

        args.translations=defaulttranslations

if routino.ParseXMLTranslations(args.translations):
    print("Error: Cannot read the translations in the file '{:s}'.".format(args.translations))
    sys.exit(1)

if args.language is not None:
    translation = routino.GetTranslation(args.language)

    if translation is None:
        list1 = routino.GetTranslationLanguages()
        list2 = routino.GetTranslationLanguageFullNames()

        print("Error: Cannot find a translation called '{:s}' in the file '{:s}'.".format(args.language,args.translations))
        print("Languages available are: {:s}".format(", ".join([i1+" ("+i2+")" for i1,i2 in zip(list1,list2)])))
        sys.exit(1)

else:
    translation = routino.GetTranslation("") # first in file

    if translation is None:
        print("Error: No translations in '{:s}'.".format(args.translations))
        sys.exit(1)


# Create the numbered waypoints

firstlatlon = True

for i in range(1,99):

    lon = getattr(args,"lon"+str(i),None)
    lat = getattr(args,"lat"+str(i),None)

    if lon is None and lat is None:
        continue

    if lon is None or lat is None:
        print("Error: All waypoints must have latitude and longitude.")
        sys.exit(1)

    if firstlatlon:
        if args.lats is not None or args.lons is not None:
            print("Error: Mixing numbered and un-numbered waypoints is not allowed.")
            sys.exit(1)
        else:
            firstlatlon = False
            args.lons = []
            args.lats = []

    args.lons.append(lon)
    args.lats.append(lat)


# Check the waypoints are valid

if args.lats is None or len(args.lats) < 2 or args.lons is None or len(args.lons) < 2:
    print("Error: At least two waypoints must be specified.")
    sys.exit(1)

if len(args.lats) != len(args.lons):
    print("Error: Number of latitudes ({:d}) and longitudes ({:d}) do not match.".format(len(lats),len(lons)))
    sys.exit(1)


# Load in the routing database

database = routino.LoadDatabase(args.dirname,args.prefix)

if database is None:
    print("Error: Could not load Routino database.")
    sys.exit(1)


# Check the profile is valid for use with this database

if routino.ValidateProfile(database,profile)!=routino.ERROR_NONE:
    print("Error: Profile is invalid or not compatible with database.")
    sys.exit(1)


# Loop through all waypoints

nwaypoints = 0
waypoints = []

for n in range(len(args.lats)):

    waypoint = routino.FindWaypoint(database, profile, args.lats[n], args.lons[n])

    if waypoint is None:
        print("Error: Cannot find node close to specified point {:d}.",n);
        sys.exit(1)

    waypoints.append(waypoint)


# Create the route

routing_options=0

if args.shortest:
    routing_options |= routino.ROUTE_SHORTEST
else:
    routing_options |= routino.ROUTE_QUICKEST

if args.html     : routing_options |= routino.ROUTE_FILE_HTML
if args.gpx_track: routing_options |= routino.ROUTE_FILE_GPX_TRACK
if args.gpx_route: routing_options |= routino.ROUTE_FILE_GPX_ROUTE
if args.text     : routing_options |= routino.ROUTE_FILE_TEXT
if args.text_all : routing_options |= routino.ROUTE_FILE_TEXT_ALL

if args.list_html    : routing_options |= routino.ROUTE_LIST_HTML
if args.list_html_all: routing_options |= routino.ROUTE_LIST_HTML_ALL
if args.list_text    : routing_options |= routino.ROUTE_LIST_TEXT
if args.list_text_all: routing_options |= routino.ROUTE_LIST_TEXT_ALL

if args.reverse: routing_options |= routino.ROUTE_REVERSE
if args.loop   : routing_options |= routino.ROUTE_LOOP

route = routino.CalculateRoute(database, profile, translation, waypoints, routing_options)

if routino.errno >= routino.ERROR_NO_ROUTE_1:
    print("Error: Cannot find a route between specified waypoints")
    sys.exit(1)

if routino.errno != routino.ERROR_NONE:
    print("Error: Internal error ({:d}).".format(routino.errno))
    sys.exit(1)


# Print the list output

if args.list_html or args.list_html_all or args.list_text or args.list_text_all:

    list=route
    first=True
    last=False

    while list:

        if list.next:
            last = False
        else:
            last = True

        print("----------------")
        print("Lon,Lat: {:.5f}, {:.5f}".format((180.0/math.pi)*list.lon,(180.0/math.pi)*list.lat))

        if args.list_html or args.list_html_all or args.list_text or args.list_text_all:
            print("Dist,Time: {:.3f} km, {:.1f} minutes".format(list.dist,list.time))

        if args.list_text_all and not first:
            print("Speed: {:0f} km/hr".format(list.speed))

        print("Point type: {:d}".format(list.type))

        if (args.list_html or args.list_html_all or args.list_text) and not first and not last:
            print("Turn: {:d} degrees".format(list.turn))

        if ((args.list_html or args.list_html_all or args.list_text) and not last) or (args.list_text_all and not first):
            print("Bearing: {:d} degrees".format(list.bearing))

        if ((args.list_html or args.list_text) and not last) or (args.list_html_all and list.name) or (args.list_text_all and not first):
            print("Name: {:s}".format(list.name))

        if args.list_html or (args.list_html_all and list.name):
            print("Desc1: {:s}".format(list.desc1))
            print("Desc2: {:s}".format(list.desc2))

            if not last:
                print("Desc3: {:s}".format(list.desc3))

        list = list.next
        first = False


# Tidy up and exit

routino.DeleteRoute(route)

routino.UnloadDatabase(database)

routino.FreeXMLProfiles()

routino.FreeXMLTranslations()

#!/bin/sh

# Main tests directory

testdir=../../src/test

# Exit on error

set -e

# Test name

name=`basename $1 .sh`

# Libroutino location

LD_LIBRARY_PATH=$testdir/..:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

# Python build location

PYTHONPATH=`echo ../build/lib.*`
export PYTHONPATH

# Create the output directory

dir=results

[ -d $dir ] || mkdir $dir

# Name related options

osm=$testdir/$name.osm
log=$name.log

option_prefix="--prefix=$name"
option_dir="--dir=$testdir/fat"

# Generic program options

option_router="--profile=motorcar --profiles=../../xml/routino-profiles.xml --translations=$testdir/copyright.xml"


# Run waypoints program

run_waypoints()
{
    perl $testdir/waypoints.pl $@
}


# Run planetsplitter

run_planetsplitter()
{
    echo "Skipping planetsplitter"
}


# Run filedumper

run_filedumper()
{
    echo "Skipping filedumper"
}


# Run the router

run_router()
{
    waypoint=$1

    shift

    [ -d $dir/$name-$waypoint ] || mkdir $dir/$name-$waypoint

    echo ../router.py $option_dir $option_prefix $option_osm $option_router $@ >> $log
         ../router.py $option_dir $option_prefix $option_osm $option_router $@ >> $log

    mv shortest* $dir/$name-$waypoint

    echo diff -u $testdir/expected/$name-$waypoint.txt $dir/$name-$waypoint/shortest-all.txt >> $log

    if $testdir/is-fast-math; then
        diff -U 0 $testdir/expected/$name-$waypoint.txt $dir/$name-$waypoint/shortest-all.txt | 2>&1 egrep '^[-+] ' || true
    else
        diff -u $testdir/expected/$name-$waypoint.txt $dir/$name-$waypoint/shortest-all.txt >> $log
    fi
}


# Run the specific test script

. $testdir/$name.sh


# Finish

exit 0

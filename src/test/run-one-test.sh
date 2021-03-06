#!/bin/sh

# Exit on error

set -e

# Test name

name=`basename $1 .sh`

shift

# Slim or non-slim

if [ "$1" = "slim" ]; then
    slim="-slim"
    dir="slim"
else
    slim=""
    dir="fat"
fi

# Libroutino or not libroutino

LD_LIBRARY_PATH=$PWD/..:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH

if [ "$2" = "lib" ]; then
    lib="+lib"
else
    lib=""
fi

# Pruned or non-pruned

if [ "$2" = "prune" ]; then
    prune=""
    pruned="-pruned"
else
    prune="--prune-none"
    pruned=""
fi

# Create the output directory

dir=$dir$lib$pruned

[ -d $dir ] || mkdir $dir

# Run the programs under a run-time debugger

debugger=${TEST_DEBUGGER:-}

# Name related options

osm=$name.osm
log=$name$lib$slim$pruned.log

option_prefix="--prefix=$name"
option_dir="--dir=$dir"

# Generic program options

option_planetsplitter="--loggable --tagging=../../xml/routino-tagging.xml --errorlog $prune"

option_filedumper="--dump-osm"

option_router="--profile=motorcar --profiles=../../xml/routino-profiles.xml --translations=copyright.xml"

if [ ! "$2" = "lib" ]; then
    option_router="$option_router --loggable"
fi


# Run waypoints program

run_waypoints()
{
    perl waypoints.pl $@
}


# Run planetsplitter

run_planetsplitter()
{
    echo "Running planetsplitter"

    echo      ../planetsplitter$slim $option_dir $option_prefix $option_planetsplitter $@ $osm > $log
    $debugger ../planetsplitter$slim $option_dir $option_prefix $option_planetsplitter $@ $osm >> $log
}


# Run filedumper

run_filedumper()
{
    echo "Running filedumper"

    echo      ../filedumper$slim $option_dir $option_prefix $option_filedumper $@ >> $log
    $debugger ../filedumper$slim $option_dir $option_prefix $option_filedumper $@ > $dir/$osm
}


# Run the router

run_router()
{
    waypoint=$1

    shift

    [ -d $dir/$name-$waypoint ] || mkdir $dir/$name-$waypoint

    echo      ../router$lib$slim $option_dir $option_prefix $option_osm $option_router $@ >> $log
    $debugger ../router$lib$slim $option_dir $option_prefix $option_osm $option_router $@ >> $log

    mv shortest* $dir/$name-$waypoint

    echo diff -u expected/$name-$waypoint.txt $dir/$name-$waypoint/shortest-all.txt >> $log

    if ./is-fast-math; then
        diff -U 0 expected/$name-$waypoint.txt $dir/$name-$waypoint/shortest-all.txt | 2>&1 egrep '^[-+] ' || true
    else
        diff -u expected/$name-$waypoint.txt $dir/$name-$waypoint/shortest-all.txt >> $log
    fi
}


# Run the specific test script

. ./$name.sh


# Finish

exit 0

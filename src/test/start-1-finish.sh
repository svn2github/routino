#!/bin/sh

# Run planetsplitter

run_planetsplitter

# Run filedumper

run_filedumper

# Waypoints

waypoints=`run_waypoints $osm list`

waypoint_start=`run_waypoints $osm WPstart 1`
waypoint_finish=`run_waypoints $osm WPfinish 3`

# Run the router for each waypoint

for waypoint in $waypoints; do

    [ ! $waypoint = "WPstart"  ] || continue
    [ ! $waypoint = "WPfinish" ] || continue

    echo "Running router : $waypoint"

    waypoint_test=`run_waypoints $osm $waypoint 2`

    run_router $waypoint $waypoint_start $waypoint_test $waypoint_finish

done

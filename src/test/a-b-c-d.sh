#!/bin/sh

# Run planetsplitter

run_planetsplitter

# Run filedumper

run_filedumper

# Waypoints

waypoints=`run_waypoints $osm list`

# Run the router for each waypoint

for waypoint in $waypoints; do

    case $waypoint in
        *a) waypoint=`echo $waypoint | sed -e 's%a$%%'` ;;
        *) continue ;;
    esac

    echo "Running router : $waypoint"

    waypoint_a=`run_waypoints $osm ${waypoint}a 1`
    waypoint_b=`run_waypoints $osm ${waypoint}b 2`
    waypoint_c=`run_waypoints $osm ${waypoint}c 3`
    waypoint_d=`run_waypoints $osm ${waypoint}d 4`

    run_router $waypoint $waypoint_a $waypoint_b $waypoint_c $waypoint_d

done

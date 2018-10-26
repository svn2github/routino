#!/bin/sh

# Run planetsplitter

run_planetsplitter

# Run filedumper

run_filedumper

# Waypoints

waypoints=`run_waypoints $osm list`

waypoint_start=`run_waypoints $osm WPstart 1`
waypoint_middle=`run_waypoints $osm WPmiddle 2`
waypoint_finish=`run_waypoints $osm WPfinish 3`

# Run the router for each loop and reverse option

for waypoint in WP WP-L WP-R WP-LR; do

    echo "Running router : $waypoint"

    option_loop=""
    option_reverse=""

    case $waypoint in
        *L*) option_loop="--loop" ;;
    esac

    case $waypoint in
        *R*) option_reverse="--reverse" ;;
    esac

    run_router $waypoint $option_loop $option_reverse $waypoint_start $waypoint_middle $waypoint_finish

done

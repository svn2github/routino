#!/bin/sh

# Run planetsplitter

run_planetsplitter

# Run filedumper

run_filedumper

# Waypoints

waypoints=`run_waypoints $osm list`

waypoint_start=`run_waypoints $osm WPstart 1`
waypoint_finish=`run_waypoints $osm WPfinish 2`

# Run the router for each profile type

profiles="motorcar bicycle"

for profile in $profiles; do

    case $profile in
        motorcar) waypoint=WP01 ;;
        *)        waypoint=WP02 ;;
    esac

    echo "Running router : $waypoint"

    run_router $waypoint --profile=$profile $waypoint_start $waypoint_finish

done

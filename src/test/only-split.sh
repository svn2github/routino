#!/bin/sh

# Pruned or non-pruned - special case

if [ "$2" = "prune" ]; then

    case $name in
        prune-isolated) prune="--prune-none --prune-isolated=100";;
        prune-short)    prune="--prune-none --prune-short=5";;
        prune-straight) prune="--prune-none --prune-straight=5";;
        *)              prune="";;
    esac

    pruned="-pruned"
else
    prune="--prune-none"
    pruned=""
fi

# Run planetsplitter

run_planetsplitter $prune

# Run filedumper

run_filedumper

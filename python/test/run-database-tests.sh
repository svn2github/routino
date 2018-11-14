#!/bin/sh

# Python build location

PYTHONPATH=`echo ../build/lib.*`
export PYTHONPATH

# Run the test

python3 ../database.py

# Finish

exit 0

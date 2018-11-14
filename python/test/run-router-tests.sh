#!/bin/sh

# Main tests directory

testdir=../../src/test

# Overall status

status=true

# Functions for running tests

run_a_test ()
{
    script=$1
    shift

    if ./run-one-test.sh $script $@ ; then
        echo "... passed"
    else
        echo "... FAILED"
        status=false
    fi
}

compare_results ()
{
    if diff -q -r $1 $2; then
        echo "... matched"
    else
        echo "... match FAILED"
        status=false
    fi
}


# Initial informational message

echo ""
$testdir/is-fast-math message


# Get the list of tests

scripts=`echo $testdir/*.osm | sed -e s/.osm/.sh/g`

# Run the scripts

for script in $scripts; do
    echo ""
    echo "Testing: $script ... "
    run_a_test $script
done


# Check results

if $status; then
    echo "Success: all tests passed"
else
    echo "Warning: Some tests FAILED"
    exit 1
fi


# Finish

exit 0

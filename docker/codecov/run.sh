#!/bin/bash

# Create a random name for the coverage report to avoid clashes in mpi
name=`cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1`

# Execute the test
$1

# Create the report
lcov --no-external --capture --base-directory /arbor-git --directory /arbor-build --output-file /shared/${name}.info &> /dev/null

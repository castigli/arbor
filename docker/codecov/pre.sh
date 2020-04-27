#!/bin/bash

# Preprocessing: create a baseline coverage report from gcno files
# with 0 hits per line

lcov --no-external --capture --initial --base-directory /arbor-git --directory /arbor-build --output-file /arbor-git/baseline-codecov.info &> /dev/null
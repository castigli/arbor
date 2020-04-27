#!/bin/bash

# Post processing: combine all *.info reports into a single one
# and prune code cov from external or generated files

TRACE_FILES_ARGS=`find /arbor -type f -iname '*.info' -exec sh -c "echo -add-tracefile {}" \;`

lcov ${TRACE_FILES_ARGS} --output-file /arbor/combined.info

# Only keep our own source
lcov --extract /arbor/combined.info "/arbor/*" --output-file /arbor/combined.info
lcov --remove /arbor/combined.info "/arbor/CMakeCXXCompilerId.cpp" --output-file /arbor/combined.info
lcov --remove /arbor/combined.info "/arbor/ext/*" --output-file /arbor/combined.info

# Upload to codecov.io
bash <(curl -s https://codecov.io/bash) -f /arbor/combined.info
#!/bin/bash

# Post processing: combine all *.info reports into a single one
# and prune code cov from external or generated files

TRACE_FILES_ARGS=`find /arbor-git -type f -iname '*.info' -exec sh -c "echo -add-tracefile {}" \;`

lcov ${TRACE_FILES_ARGS} --output-file /arbor-git/combined.info

# Only keep our own source
lcov --extract /arbor-git/combined.info "/arbor-git/*" --output-file /arbor-git/combined.info
lcov --remove /arbor-git/combined.info "/arbor-git/CMakeCXXCompilerId.cpp" --output-file /arbor-git/combined.info
lcov --remove /arbor-git/combined.info "/arbor-git/ext/*" --output-file /arbor-git/combined.info

# Upload to codecov.io
pushd /arbor-git
bash <(curl -s https://codecov.io/bash) -f /arbor-git/combined.info
popd

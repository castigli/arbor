# Multistage build: here we import the current source code
# into build environment image, build the project, bundle it
# and then extract it into a small image that just contains
# the binaries we need to run

ARG BUILD_ENV

FROM $BUILD_ENV as builder

# Build arbor
COPY . /arbor-git

# Build and bundle binaries
RUN mkdir /arbor-build && cd /arbor-build && \
    CC=mpicc CXX=mpicxx cmake /arbor-git \
      -DARB_VECTORIZE=ON \
      -DARB_ARCH=broadwell \
      -DARB_WITH_PYTHON=OFF \
      -DARB_WITH_MPI=ON \
      -DARB_GPU=cuda \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-g -O0 -fprofile-arcs -ftest-coverage" \
      -DCMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage" \
      -DCMAKE_INSTALL_PREFIX=/usr && \
    make -j$(nproc) tests && \
    # Copy the binaries over
    /root/libtree/libtree --chrpath \
      -d /root/arbor.bundle \
      $(which gcov) \
      /arbor-build/bin/modcc \
      /arbor-build/bin/unit \
      /arbor-build/bin/unit-local \
      /arbor-build/bin/unit-modcc \
      /arbor-build/bin/unit-mpi && \ 
    # Copy lcov over (it's perl scripts, so cannot use libtree)
    cp -L $(which lcov) $(which geninfo) /root/arbor.bundle/usr/bin && \
    # Remove everything except for gcno coverage files
    mv /arbor-build /arbor-build-tmp && \
    mkdir /arbor-build && \
    cd /arbor-build-tmp && \
    find -iname "*.gcno" -exec cp --parent \{\} /arbor-build \; && \
    rm -rf /arbor-build-tmp /arbor-git

FROM ubuntu:18.04

# Install perl to make lcov happy
# codecov upload needs curl + ca-certificates
# TODO: remove git after https://github.com/codecov/codecov-bash/pull/291 
#       or https://github.com/codecov/codecov-bash/pull/265 is merged
RUN apt-get update && \
    apt-get install --no-install-recommends -qq \
      perl \
      curl \
      ca-certificates \
      git && \
    rm -rf /var/lib/apt/lists/*

# This is the only thing necessary really from nvidia/cuda's ubuntu18.04 runtime image
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility
ENV NVIDIA_REQUIRE_CUDA "cuda>=10.1 brand=tesla,driver>=384,driver<385 brand=tesla,driver>=396,driver<397 brand=tesla,driver>=410,driver<411"

# Copy the executables and the codecov gcno files
COPY --from=builder /root/arbor.bundle /root/arbor.bundle
COPY --from=builder /arbor-build /arbor-build

# Make it easy to call our binaries.
ENV PATH="/root/arbor.bundle/usr/bin:$PATH"

RUN echo "/root/arbor.bundle/usr/lib/" > /etc/ld.so.conf.d/arbor.conf && ldconfig

WORKDIR /root/arbor.bundle/usr/bin


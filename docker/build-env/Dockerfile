FROM nvidia/cuda:10.1-devel-ubuntu18.04

WORKDIR /root

ARG MPICH_VERSION=3.1.4

ENV DEBIAN_FRONTEND noninteractive
ENV FORCE_UNSAFE_CONFIGURE 1
ENV MPICH_VERSION ${MPICH_VERSION}

# Install basic tools
RUN apt-get update -qq && apt-get install -qq -y --no-install-recommends \
    build-essential \
    python \
    git tar wget curl && \
    rm -rf /var/lib/apt/lists/*

# Install cmake
RUN wget -qO- "https://cmake.org/files/v3.17/cmake-3.17.0-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

# Install MPICH ABI compatible with Cray's lib on Piz Daint
RUN wget -q https://www.mpich.org/static/downloads/${MPICH_VERSION}/mpich-${MPICH_VERSION}.tar.gz && \
    tar -xzf mpich-${MPICH_VERSION}.tar.gz && \
    cd mpich-${MPICH_VERSION} && \
    ./configure --disable-fortran && \
    make install -j$(nproc) && \
    rm -rf mpich-${MPICH_VERSION}.tar.gz mpich-${MPICH_VERSION}

# Install bundle tooling for creating small Docker images
RUN wget -q https://github.com/haampie/libtree/releases/download/v1.0.3/libtree_x86_64.tar.gz && \
    tar -xzf libtree_x86_64.tar.gz && \
    rm libtree_x86_64.tar.gz

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    locales \
    build-essential \
    git \
    python3 \
    python3-pip \
    cmake \
    make \
    wget \
    gdb \
    gdb-multiarch \
    valgrind \
    ca-certificates \
    binutils-multiarch \
    clang-format \
    clang-tidy \
    openssh-server \
    iproute2 \
    unzip \
    nano \
    doxygen \
    curl \
    file \
    && sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen \
    && locale-gen en_US.UTF-8 \
    && update-locale LANG=en_US.UTF-8 \
    && pip3 install --no-cache-dir pre-commit \
    && rm -rf /var/lib/apt/lists/*

# Set locale variables
ENV LANG=en_US.UTF-8
ENV LANGUAGE=en_US:en
ENV LC_ALL=en_US.UTF-8

# Install toolchain
ARG SDKFILE=*-hammerhead-toolchain-*.sh
COPY ./${SDKFILE} /tmp/
RUN printf '\ny\n' | bash "$(find /tmp -maxdepth 1 -type f -name ${SDKFILE} -print -quit)" \
    rm -r /tmp/${SDKFILE}

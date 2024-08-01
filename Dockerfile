FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install cmake ninja-build clang-18 libc++-18-dev clang-tools-18 lldb-18 imagemagick --no-install-recommends -y && \
    rm -rf /var/lib/apt/lists/*

CMD ["/bin/sh"]
#!/usr/bin/env bash

cd SMTPServer
mkdir -p build
make exe_directory="../../" \
    compile_flags="-g -g3 -O0 -D_GNU_SOURCE -DDEBUG" \
    --directory=build \
    --makefile=../Makefile || exit 1
 
cd ../SMTPClient
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_DEBUG=ON . || exit 3
make || exit 4
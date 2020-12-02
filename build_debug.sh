#!/usr/bin/env bash

cd SMTPServer 
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_DEBUG=ON . || exit 1
make || exit 2
 
cd ../SMTPClient
cmake -DCMAKE_BUILD_TYPE=Debug -DUSE_DEBUG=ON . || exit 3
make || exit 4
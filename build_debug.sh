#!/usr/bin/env bash

cd SMTPServer 
cmake -DCMAKE_BUILD_TYPE=Debug .
make

cd ../SMTPClient
cmake -DCMAKE_BUILD_TYPE=Debug .
make
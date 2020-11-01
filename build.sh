#!/usr/bin/env bash

cd SMTPServer
cmake .
make

cd ../SMTPClient
cmake .
make
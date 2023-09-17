#!/bin/bash

# Setup emscripten
cd extern/emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ../..

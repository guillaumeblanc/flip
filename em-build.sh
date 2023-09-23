#!/bin/bash

# Setup emscripten
cd extern/emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
cd ../..


# Setup cmake
mkdir build-wasm
cd build-wasm
emcmake cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build .
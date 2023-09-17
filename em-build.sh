#!/bin/bash

# Setup emscripten
source ./em-setup.sh

# Setup cmake
mkdir build-wasm
cd build-wasm
emcmake cmake ..

# Build
cmake --build .
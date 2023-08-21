cmake_minimum_required(VERSION 3.20)

# Defines the project's name
project(flip)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Starts building the sources tree
add_subdirectory(src)
add_subdirectory(samples)

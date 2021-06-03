cmake_minimum_required(VERSION 3.10)

# set the project name
project(magic_enum VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE .)

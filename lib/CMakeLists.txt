cmake_minimum_required(VERSION 3.10)

# set the project name
project(RTL-lib VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_library(RTL-Lib SHARED isa/pep10/model.cpp isa/pep10/instruction.cpp)
# Allow version header to be included
target_include_directories(RTL-Lib PUBLIC "${PROJECT_BINARY_DIR}/")
target_include_directories(RTL-Lib PUBLIC "${PROJECT_SOURCE_DIR}/")

target_link_libraries(RTL-Lib magic_enum)
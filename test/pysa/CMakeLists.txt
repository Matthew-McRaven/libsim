cmake_minimum_required(VERSION 3.10)

# set the project name
project(test-pybind VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Allow version header to be included
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")
add_executable (test-pybind ${sources})

target_link_libraries(test-pybind RTL-Lib catch magic_enum)

add_test(NAME test-pybind COMMAND test-pybind)

find_package(Python3 COMPONENTS Interpreter VERSION 3.9)
if(Python3_Interpreter_FOUND)
  add_test(NAME test-pythonlib COMMAND Python3::Interpreter -m pytest ${CMAKE_CURRENT_SOURCE_DIR})
endif()
cmake_minimum_required(VERSION 3.10)

# set the project name
project(test VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Allow version header to be included
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp")
add_executable (Test ${sources})

target_link_libraries(Test RTL-Lib catch magic_enum)
#link_directories("${PROJECT_BINARY_DIR}/../lib")

add_test (NAME test-all COMMAND Test)
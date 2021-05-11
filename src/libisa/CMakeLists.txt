cmake_minimum_required(VERSION 3.10)

# set the project name
project(RTL-lib VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp")
# add the executable
add_library(RTL-Lib STATIC ${sources})

target_include_directories(RTL-Lib PUBLIC ${Boost_INCLUDE_DIR} )
target_link_libraries( RTL-Lib PUBLIC ${Boost_LIBRARIES} )

# Allow version header to be included
target_include_directories(RTL-Lib PUBLIC "${PROJECT_BINARY_DIR}/")
target_include_directories(RTL-Lib PUBLIC "${PROJECT_SOURCE_DIR}/")

# TODO: Switch to std::ranges when supported (maybe clang 13).
target_link_libraries(RTL-Lib PUBLIC magic_enum fmt ngraph elfio)
cmake_minimum_required(VERSION 3.10)

# Run clang tidy iff it is installed and on the path.
find_program(CLANG_TIDY "clang-tidy")
# Emscripten seems to screw with clang-tidy's ability to work.
if(CLANG_TIDY AND  (NOT EMSCRIPTEN))
	set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,clang-analyzer-*)
endif()

# set the project name
project(RTL-lib VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp")
# add the executable
add_library(RTL-Lib STATIC ${sources})

if(code_coverage)
	# Add code coverage support
	SET(CMAKE_CXX_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
	SET(CMAKE_C_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
endif()

target_include_directories(RTL-Lib PUBLIC ${Boost_INCLUDE_DIR} )
target_link_libraries( RTL-Lib PUBLIC ${Boost_LIBRARIES} builtins)

# Allow version header to be included
target_include_directories(RTL-Lib PUBLIC "${PROJECT_BINARY_DIR}/")
target_include_directories(RTL-Lib PUBLIC "${PROJECT_SOURCE_DIR}/")

# TODO: Switch to std::ranges when supported (maybe clang 13).
target_link_libraries(RTL-Lib PUBLIC magic_enum fmt ngraph elfio outcome cereal)
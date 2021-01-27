cmake_minimum_required(VERSION 3.10)

# set the project name
project(test-lib VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Allow version header to be included
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp")
add_executable (test-lib ${sources})

if(EMSCRIPTEN)
	set_target_properties(test-lib PROPERTIES LINK_FLAGS " -s DISABLE_EXCEPTION_CATCHING=2")
	# Disable unit tests that throw exceptions, because exceptions can't be caught inside wasm.
	target_compile_definitions(test-lib PRIVATE EMSCRIPTEN=1)
endif()


target_link_libraries(test-lib RTL-Lib catch magic_enum)

add_test (NAME test-lib COMMAND test-lib)
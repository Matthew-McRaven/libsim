cmake_minimum_required(VERSION 3.10)

# set the project name
project(test-isa-definitions VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Allow version header to be included
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "**/*.cpp" "*.cpp")
add_executable (test-isa-definitions ${sources})

if(EMSCRIPTEN)
	set_target_properties(test-isa-definitions PROPERTIES LINK_FLAGS " -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1")
	# Dtest-isable unit tests that throw exceptions, because exceptions can't be caught inside wasm.
	target_compile_definitions(test-isa-definitions PRIVATE EMSCRIPTEN=1)
endif()


target_link_libraries(test-isa-definitions RTL-Lib catch magic_enum builtins)

add_test (NAME test-isa-definitions COMMAND test-isa-definitions)
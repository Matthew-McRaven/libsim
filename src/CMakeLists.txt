add_subdirectory(libisa)
add_subdirectory(sample)
# If we are using emscripten, don't build python bindings!
# Additionally, we need to compile C++ interop using embind.

if(EMSCRIPTEN)
	add_subdirectory(nodeisa)
else()
endif()

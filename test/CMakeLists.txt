add_subdirectory(libisa)

# If we are using emscripten, don't build python bindings!
# Additionally, we need to compile C++ interop using embind.
if(EMSCRIPTEN)
	add_subdirectory(nodeisa)
else()
endif()

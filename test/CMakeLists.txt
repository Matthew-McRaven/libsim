add_subdirectory(libisa)

find_package(PythonLibs 3.9)

# If we are using emscripten, don't build python bindings!
# Additionally, we need to compile C++ interop using embind.
if(EMSCRIPTEN)
	add_subdirectory(nodeisa)
else()
	if (PYTHONINTERP_FOUND)
		add_subdirectory(pysa)
	endif()	
endif()

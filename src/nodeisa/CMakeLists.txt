cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(pep9 pep9.cpp)
add_executable(pep10 pep10.cpp)
add_executable(sim sim.cpp)
# Allow version header to be included
target_link_libraries(pep9 RTL-Lib)
target_link_libraries(pep10 RTL-Lib)
target_link_libraries(sim RTL-Lib)
set_target_properties(pep9 PROPERTIES LINK_FLAGS " -s DEMANGLE_SUPPORT=1 -s DISABLE_EXCEPTION_CATCHING=0 -s MODULARIZE --bind")
set_target_properties(pep10 PROPERTIES LINK_FLAGS " -s DEMANGLE_SUPPORT=1 -s DISABLE_EXCEPTION_CATCHING=0 -s MODULARIZE --bind")
set_target_properties(sim PROPERTIES LINK_FLAGS " -s DEMANGLE_SUPPORT=1   -s DISABLE_EXCEPTION_CATCHING=0 -s MODULARIZE --bind")

file(COPY node/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(nodeisa application.cpp)
# Allow version header to be included
target_link_libraries(nodeisa RTL-Lib)
set_target_properties(nodeisa PROPERTIES LINK_FLAGS " -s DEMANGLE_SUPPORT=1  -s MODULARIZE=1 --bind")

file(COPY node/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY node/. DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

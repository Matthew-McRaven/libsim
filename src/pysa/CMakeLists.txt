cmake_minimum_required(VERSION 3.5)

project(pywrap LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

FIND_PACKAGE(PythonInterp 3.9 REQUIRED)
set (Boost_DETAILED_FAILURE_MSG ON)
set (Boost_DEBUG ON)  #<---------- Real life saver
set(Boost_USE_STATIC_RUNTIME ON)

find_package(PythonLibs 3.9 REQUIRED)
if (PYTHONINTERP_FOUND)
  if (UNIX AND NOT APPLE)
    if (PYTHON_VERSION_MAJOR EQUAL 3)
        FIND_PACKAGE(Boost 1.75.0 COMPONENTS python39 REQUIRED)
        FIND_PACKAGE(PythonInterp 3.9 REQUIRED)
        FIND_PACKAGE(PythonLibs 3 REQUIRED)
    else()
        FIND_PACKAGE(Boost 1.75.0 COMPONENTS python39 REQUIRED)
        FIND_PACKAGE(PythonInterp 3.9)
        FIND_PACKAGE(PythonLibs 3.9 REQUIRED)
    endif()
  else()	
    if (PYTHON_VERSION_MAJOR EQUAL 3)
        FIND_PACKAGE(Boost 1.75.0 COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
        FIND_PACKAGE(PythonInterp 3.9)
        FIND_PACKAGE(PythonLibs 3 REQUIRED)
    else()
        FIND_PACKAGE(Boost 1.75.0 COMPONENTS python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
        FIND_PACKAGE(PythonInterp 3.9)
        FIND_PACKAGE(PythonLibs 3.9 REQUIRED)
    endif()
  endif()
else()
    message("Python not found")
endif()

message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")

ENABLE_TESTING()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})

link_directories(${PYTHON_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})
PYTHON_ADD_MODULE(cutils SHARED dummy.cpp)
target_link_libraries(cutils libboost_python39.a)
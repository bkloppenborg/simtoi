cmake_minimum_required(VERSION 2.8) 
project(simtoi)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -D DEBUG")

# Call CMake on the subdirectories
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

# Add librariy subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/textio)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/liboi)
add_subdirectory(src)


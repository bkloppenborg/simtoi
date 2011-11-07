cmake_minimum_required(VERSION 2.8) 
project(simtoi)

# Call CMake on the subdirectories

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

add_subdirectory(src)
add_subdirectory(lib)
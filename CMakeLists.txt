cmake_minimum_required(VERSION 2.8) 
project(simtoi)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS_DEBUG "-g -D DEBUG -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Call CMake on the subdirectories
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

# Include textio
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/textio)
include_directories(${CMAKE_SOURCE_DIR}/lib/textio/include)

# Include liboi, getoifits, oifitslib
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/liboi)
include_directories(${CMAKE_SOURCE_DIR}/lib/liboi/src)
include_directories(${CMAKE_SOURCE_DIR}/lib/liboi/lib/getoifits)
include_directories(${CMAKE_SOURCE_DIR}/lib/liboi/lib/oifitslib)

# Include multinest
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/multinest)
include_directories(${CMAKE_SOURCE_DIR}/lib/multinest)

# Include cmpfit (version 1.2)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/cmpfit-1.2)
include_directories(${CMAKE_SOURCE_DIR}/lib/cmpfit-1.2)

add_subdirectory(src)


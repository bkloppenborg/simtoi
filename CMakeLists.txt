cmake_minimum_required(VERSION 2.8) 
project(simtoi)

# Compile with C++11 support.
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# If MultiNest isn't installed to a standard location, provide a hint
# to where it can be found.  Comment out otherwise
#SET(MULTINEST_ROOT_HINT /homes/bkloppen/local)

# If a library is in a non-standard location, add it to the search path here:
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib/fglrx)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -D DEBUG -O0")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wshadow")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Set some CMake properties:
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    CACHE PATH "Single directory for all executable and dynamic libraries on Windows.")
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules ${PROJECT_SOURCE_DIR}/lib/liboi/CMakeModules)
include_directories(${CMAKE_SOURCE_DIR}/include)

# We need to compile the following libraries
# jsoncpp
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/jsoncpp EXCLUDE_FROM_ALL)
include_directories(${CMAKE_SOURCE_DIR}/lib/jsoncpp/include)

FIND_PACKAGE(LAPACK)
if(LAPACK_FOUND)
    # levmar (v. 2.6)
    add_subdirectory(${CMAKE_SOURCE_DIR}/lib/levmar-2.6 EXCLUDE_FROM_ALL)
    include_directories(${CMAKE_SOURCE_DIR}/lib/levmar-2.6)
endif(LAPACK_FOUND)

# textio
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/textio EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/lib/textio/src)

# ccoifits
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/ccoifits EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/lib/ccoifits/src/calibrators)
include_directories(${PROJECT_SOURCE_DIR}/lib/ccoifits/src)
include_directories(${CCOIFITS_INCLUDE_DIRS})

# liboi
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/liboi EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/lib/liboi/src)
include_directories(${LIBOI_INCLUDE_DIRS})

# healpix
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/chealpix EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/lib/chealpix)
include_directories(${CHEALPIX_INCLUDE_DIRS})

# Build the main directory, always
add_subdirectory(src)

# Copy over kernel and shader sources:
file(GLOB KERNELS ${CMAKE_SOURCE_DIR}/lib/liboi/src/kernels/*.cl)
file(COPY ${KERNELS} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/kernels/)

file(GLOB SHADERS_FRAG ${CMAKE_SOURCE_DIR}/src/shaders/*.glsl)
file(COPY ${SHADERS_FRAG} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/shaders/)

file(GLOB SHADERS_CONFIG ${CMAKE_SOURCE_DIR}/src/shaders/*.json)
file(COPY ${SHADERS_CONFIG} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/shaders/)

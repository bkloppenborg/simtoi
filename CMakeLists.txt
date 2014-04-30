cmake_minimum_required(VERSION 2.8) 
project(simtoi)

ADD_DEFINITIONS("-std=c++0x")

# If MultiNest isn't installed to a standard location, provide a hint
# to where it can be found.  Comment out otherwise
SET(MULTINEST_ROOT_HINT /homes/bkloppen/local)

# If a library is in a non-standard location, add it to the search path here:
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib64/fglrx)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -D DEBUG -O0")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wshadow")
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Set some CMake properties:
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
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

cmake_minimum_required(VERSION 2.8) 
project(simtoi)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIRS})
find_package(Boost REQUIRED)

# Add include directories and libs for compiled libraries.
include_directories(${CMAKE_SOURCE_DIR}/lib/textio/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/liboi/src)

# TODO: Temporary inclusion for oifitslib and getoifits:
# Temporary, required for oifitslib.  Don't forget to remove from below too!
find_package(CFITSIO REQUIRED)
include_directories(${CFITSIO_INCLUDE_DIRS})
find_package(GTK2 REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/lib/liboi/lib/getoifits)
include_directories(${CMAKE_SOURCE_DIR}/lib/liboi/lib/oifitslib)

# Enable prototype functions in OpenGL (even though some are no longer prototypes)
set(CMAKE_CXX_FLAGS "-DGL_GLEXT_PROTOTYPES")

# Print out a status message to indicate if OpenGL, GLUT and GLEW were found.
message(STATUS ${GLUT_glut_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})

# Assemble all of the source to build simtoi.
# Link against GLUT, GLEW, OpenGL, and OpenCL
file(GLOB SOURCE *.cpp)
add_executable(simtoi ${SOURCE})
target_link_libraries(simtoi textio oi ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY} ${OPENCL_LIBRARIES} ${Boost_LIBRARIES})

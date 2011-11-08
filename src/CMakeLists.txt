cmake_minimum_required(VERSION 2.8) 

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
#find_package(OpenCL REQUIRED)
#include_directories(${OPENCL_INCLUDE_DIRS})

# Enable prototype functions in OpenGL (even though some are no longer prototypes)
set(CMAKE_CXX_FLAGS "-DGL_GLEXT_PROTOTYPES")

# Print out a status message to indicate if OpenGL, GLUT and GLEW were found.
message(STATUS ${GLUT_glut_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})

# Assemble all of the source to build simtoi.
# Link against GLUT, GLEW, OpenGL, and OpenCL
file(GLOB SOURCE *.cpp)
add_executable(simtoi ${SOURCE})
target_link_libraries(simtoi ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
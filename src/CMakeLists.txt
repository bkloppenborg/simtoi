cmake_minimum_required(VERSION 2.8) 
project(simtoi CXX C Fortran)

# include directories for the oilib:
include_directories(${OI_INCLUDE_DIRS})

# OpenGL is required
if(NOT OpenGL_FOUND)
	find_package(OpenGL REQUIRED)
	if(OPENGL_FOUND)
	    MESSAGE(STATUS "found OPENGL, lib = " ${OPENGL_LIBRARIES} )
	endif(OPENGL_FOUND)
endif(NOT OpenGL_FOUND)
add_definitions(-DGL_GLEXT_PROTOTYPES )

if (CMAKE_BUILD_TOOL MATCHES "make")
    #add_definitions(-Wall  -Wno-deprecated -Werror -Wshadow )
endif (CMAKE_BUILD_TOOL MATCHES "make")

find_package(Qt4 COMPONENTS QtCore QtGui QtMain QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/QT ${CMAKE_CURRENT_BINARY_DIR}/QT)
add_subdirectory(QT) 

# Assemble all of the source to build simtoi.
file(GLOB SOURCE *.cpp)
add_executable(simtoi ${SOURCE})
SET_TARGET_PROPERTIES(simtoi PROPERTIES LINKER_LANGUAGE Fortran)
target_link_libraries(simtoi QT_files textio jsoncpp oi cmpfit multinest levmar ${QT_LIBRARIES} ${OPENGL_LIBRARIES})

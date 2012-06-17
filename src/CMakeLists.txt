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
file(GLOB SOURCE *.cpp models/*.cpp)

# Find MultiNest.  If MultiNest isn't installed to a standard location, provide a hint
# to where it can be found:
SET(MULTINEST_ROOT_HINT /home/bkloppenborg/workspace/multinest)

find_package(MultiNest)
if(MULTINEST_FOUND)
    include_directories(${MULTINEST_INCLUDE_DIRS})
    LIST(APPEND OPTIONAL_LIBS ${MULTINEST_LIBRARIES})
    message(STATUS "MultiNest found, SIMTOI will be compiled with support for this minimizer.")
    add_definitions(-D_ADD_MULTINEST)
else(MULTINEST_FOUND)
    message(STATUS "MultiNest NOT found, SIMTOI compiled without support for this minimizer.")
    file(GLOB REMOVE_MULTINEST "CMinimizer_MultiNest.cpp")
    list(REMOVE_ITEM SOURCE ${REMOVE_MULTINEST})
endif(MULTINEST_FOUND)

# Now add the binary
add_executable(simtoi ${SOURCE})

SET_TARGET_PROPERTIES(simtoi PROPERTIES LINKER_LANGUAGE Fortran)
target_link_libraries(simtoi QT_files textio jsoncpp oi levmar ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${OPTIONAL_LIBS})

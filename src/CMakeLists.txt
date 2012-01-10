cmake_minimum_required(VERSION 2.8) 
project(simtoi CXX C Fortran)

# OpenGL is required
if(NOT OpenGL_FOUND)
	find_package(OpenGL REQUIRED)
endif(NOT OpenGL_FOUND)
set(CMAKE_CXX_FLAGS "-DGL_GLEXT_PROTOTYPES")

# QT is required:
find_package(Qt4 COMPONENTS QtCore QtGui QtMain QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ui ${CMAKE_CURRENT_BINARY_DIR}/ui)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/threads ${CMAKE_CURRENT_BINARY_DIR}/threads)
add_subdirectory(ui)
#add_subdirectory(threads)

# include directories for the oilib:
include_directories(${OI_INCLUDE_DIRS})

# Assemble all of the source to build simtoi.
file(GLOB SOURCE *.cpp)
add_executable(simtoi ${SOURCE})
target_link_libraries(simtoi ui textio oi multinest ${QT_LIBRARIES})

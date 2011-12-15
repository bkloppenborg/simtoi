cmake_minimum_required(VERSION 2.8) 
project(simtoi CXX C Fortran)

# OpenGL is required
find_package(OpenGL REQUIRED)
add_subdirectory(ui)

# include directories for the oilib:
include_directories(${OI_INCLUDE_DIRS})
find_package(Qt4 COMPONENTS QtCore QtGui QtMain QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(./ui ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ui)

# Enable prototype functions in OpenGL (even though some are no longer prototypes)
set(CMAKE_CXX_FLAGS "-DGL_GLEXT_PROTOTYPES")

# Assemble all of the source to build simtoi.
file(GLOB SOURCE *.cpp)
file(GLOB UI_SOURCE ui/*.cpp)
add_executable(simtoi ${SOURCE})
target_link_libraries(simtoi simtoi_ui textio oi multinest ${QT_LIBRARIES})

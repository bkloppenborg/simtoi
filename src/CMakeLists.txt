cmake_minimum_required(VERSION 2.8)
project(simtoi CXX C)

find_package(OpenGL REQUIRED)
MESSAGE(STATUS "found OPENGL, lib = " ${OPENGL_gl_LIBRARY} )
add_definitions(-DGL_GLEXT_PROTOTYPES )

set(GL_PATH_SUFFIX "")
if(APPLE)
    set(GL_PATH_SUFFIX "/Libraries")
endif()

# Check for specific OpenGL functionality
include(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(GL glFramebufferTexture
    "${OPENGL_gl_LIBRARY}${GL_PATH_SUFFIX}" HAVE_GL_FRAMEBUFFER_TEXTURE)
if(HAVE_GL_FRAMEBUFFER_TEXTURE)
    add_definitions(-DHAVE_GL_FRAMEBUFFER_TEXTURE)
endif(HAVE_GL_FRAMEBUFFER_TEXTURE)

CHECK_LIBRARY_EXISTS(GL glFramebufferTexture2D
    "${OPENGL_gl_LIBRARY}${GL_PATH_SUFFIX}" HAVE_GL_FRAMEBUFFER_TEXTURE_2D)
if(HAVE_GL_FRAMEBUFFER_TEXTURE_2D)
    add_definitions(-DHAVE_GL_FRAMEBUFFER_TEXTURE_2D)
endif(HAVE_GL_FRAMEBUFFER_TEXTURE_2D)

# Include the OpenGL Mathematics Library
find_package(GLM REQUIRED)
INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})

# QT library
SET(CMAKE_AUTOMOC ON)
FIND_PACKAGE(Qt5 COMPONENTS Core Widgets Gui OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${Qt5_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})
SET(QT_LIBRARIES ${Qt5_LIBRARIES} ${Qt5Core_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Gui_LIBRARIES} ${Qt5OpenGL_LIBRARIES})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/QT ${CMAKE_CURRENT_BINARY_DIR}/QT)
add_subdirectory(QT)

# Define the version of QT found
add_definitions( -DQT_VERSION_MAJOR=${QT_VERSION_MAJOR} )
add_definitions( -DQT_VERSION_MINOR=${QT_VERSION_MINOR} )

add_subdirectory(scripts)
add_subdirectory(models)
add_subdirectory(features)
add_subdirectory(minimizers)
add_definitions(${MINIMISER_DEFINITIONS})

file(GLOB SOURCE *.cpp
positions/*.cpp tasks/*.cpp)

# Now add the binary
add_executable(simtoi ${SOURCE})
target_link_libraries(simtoi simtoi_models simtoi_minimizers simtoi_features
    QT_files jsoncpp_lib levmar oi textio chealpix
    ${QT_LIBRARIES} ${OPENGL_LIBRARIES})

# install step
install(TARGETS simtoi DESTINATION bin)

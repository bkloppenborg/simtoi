cmake_minimum_required(VERSION 2.8)
project(simtoi CXX C)

find_package(OpenGL REQUIRED)
MESSAGE(STATUS "found OPENGL, lib = " ${OPENGL_LIBRARIES} )
add_definitions(-DGL_GLEXT_PROTOTYPES )

# Check for specific OpenGL functionality
include(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(GL glFramebufferTexture ${OPENGL_gl_LIBRARY} HAVE_GL_FRAMEBUFFER_TEXTURE)
if(HAVE_GL_FRAMEBUFFER_TEXTURE)
    add_definitions(-DHAVE_GL_FRAMEBUFFER_TEXTURE)
endif(HAVE_GL_FRAMEBUFFER_TEXTURE)

CHECK_LIBRARY_EXISTS(GL glFramebufferTexture2D ${OPENGL_gl_LIBRARY} HAVE_GL_FRAMEBUFFER_TEXTURE_2D)
if(HAVE_GL_FRAMEBUFFER_TEXTURE_2D)
    add_definitions(-DHAVE_GL_FRAMEBUFFER_TEXTURE_2D)
endif(HAVE_GL_FRAMEBUFFER_TEXTURE_2D)

# Include the OpenGL Mathematics Library
find_package(GLM REQUIRED)
INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})

find_package(Qt4 COMPONENTS QtCore QtGui QtMain QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/QT ${CMAKE_CURRENT_BINARY_DIR}/QT)
add_subdirectory(QT)

# Define the version of QT found
add_definitions( -DQT_VERSION_MAJOR=${QT_VERSION_MAJOR} )
add_definitions( -DQT_VERSION_MINOR=${QT_VERSION_MINOR} )

INCLUDE_DIRECTORIES(.)
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


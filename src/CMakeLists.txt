cmake_minimum_required(VERSION 2.8) 
project(simtoi CXX C Fortran)

find_package(OpenGL REQUIRED)
MESSAGE(STATUS "found OPENGL, lib = " ${OPENGL_LIBRARIES} )
add_definitions(-DGL_GLEXT_PROTOTYPES )

find_package(Qt4 COMPONENTS QtCore QtGui QtMain QtOpenGL REQUIRED)
INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/QT ${CMAKE_CURRENT_BINARY_DIR}/QT)
add_subdirectory(QT) 

add_subdirectory(scripts)

# Assemble all of the source to build simtoi.
file(GLOB SOURCE *.cpp models/*.cpp)

# Levmar requires a linear algebra package, but it doesn't export a
# LEVMAR_LIBRARIES. SIMTOI instructions say install Lapack, so we'll always
# link against that;
FIND_PACKAGE(LAPACK REQUIRED)

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
target_link_libraries(simtoi QT_files jsoncpp levmar oi_static textio_static ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${LAPACK_LIBRARIES} ${OPTIONAL_LIBS})

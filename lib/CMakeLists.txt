cmake_minimum_required(VERSION 2.8) 
project(simtoi)

#include(ExternalProject)
#ExternalProject_Add(textio
#	PREFIX ${CMAKE_SOURCE_DIR}/lib/textio
#    SVN_REPOSITORY -r 5 http://finiteline.homeip.net/svn/textio
#    SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/textio/src
#    CONFIGURE_COMMAND cmake ..
#    INSTALL_COMMAND ""
#)

## Push the locations of the include and lib directories into parent scope.
#SET(TEXTIO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textio/src/include PARENT_SCOPE)
#SET(TEXTIO_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/textio/src/lib PARENT_SCOPE)

add_subdirectory(textio)

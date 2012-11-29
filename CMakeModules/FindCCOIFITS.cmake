# - Find CCOIFITS
# Once done this will define
#  CCOIFITS_FOUND - System has CCOIFITS
#  CCOIFITS_INCLUDE_DIRS - The CCOIFITS include directories
#  CCOIFITS_LIBRARIES - The libraries needed to use CCOIFITS (none)

find_package(CCFITS REQUIRED)

FIND_LIBRARY(CCOIFITS_LIBRARY 
    NAMES ccoifits
    HINTS ${CCOIFITS_HINT}/lib ${CMAKE_INSTALL_PREFIX}/lib
    DOC "CCOIFITS library.")

FIND_PATH(CCOIFITS_INCLUDE_DIR 
    NAMES ccoifits.hpp
    HINTS ${CCOIFITS_HINT}/include ${CMAKE_INSTALL_PREFIX}/include
    DOC "CCOIFITS include directory.")

set(CCOIFITS_LIBRARIES ${CCOIFITS_LIBRARY} ${CCFITS_LIBRARY})
set(CCOIFITS_INCLUDE_DIRS ${CCOIFITS_INCLUDE_DIR} ${CCFITS_INCLUDE_DIR})

message(STATUS ${CCOIFITS_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CCOIFITS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CCOIFITS DEFAULT_MSG
                                  CCOIFITS_LIBRARY CCOIFITS_INCLUDE_DIR CCFITS_INCLUDE_DIRS CCFITS_LIBRARIES)

mark_as_advanced(CCOIFITS_INCLUDE_DIR CCOIFITS_LIBRARY)

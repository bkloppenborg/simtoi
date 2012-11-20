# - Find LIBOI
# Once done this will define
#  LIBOI_FOUND - System has LIBOI
#  LIBOI_INCLUDE_DIRS - The LIBOI include directories
#  LIBOI_LIBRARIES - The libraries needed to use LIBOI (none)

find_package(TEXTIO REQUIRED)
find_package(CCOIFITS REQUIRED)

FIND_LIBRARY(LIBOI_LIBRARY 
    NAMES oi
    HINTS ${LIBOI_HINT}/lib ${CMAKE_INSTALL_PREFIX}/lib
    DOC "LIBOI library.")

FIND_PATH(LIBOI_INCLUDE_DIR 
    NAMES oi.hpp
    HINTS ${LIBOI_HINT}/include ${CMAKE_INSTALL_PREFIX}/include
    DOC "LIBOI include directory.")

set(LIBOI_LIBRARIES ${LIBOI_LIBRARY} ${TEXTIO_LIBRARY} ${CCOIFITS_LIBRARY})
set(LIBOI_INCLUDE_DIRS ${LIBOI_INCLUDE_DIR} ${TEXTIO_INCLUDE_DIR} ${CCOIFITS_INCLUDE_DIR})

message(STATUS ${LIBOI_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBOI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBOI DEFAULT_MSG
                                  LIBOI_LIBRARY LIBOI_INCLUDE_DIR CCFITS_INCLUDE_DIRS CCFITS_LIBRARIES)

mark_as_advanced(LIBOI_INCLUDE_DIR LIBOI_LIBRARY)

# - Try to find LibXml2
# Once done this will define
#  TEXTIO_FOUND - System has textio
#  TEXTIO_INCLUDE_DIRS - The textio include directories
#  TEXTIO_LIBRARIES - The libraries needed to use textio (none)
    
FIND_LIBRARY(TEXTIO_LIBRARY 
    NAMES textio
    HINTS ${TEXTIO_HINT}/lib ${CMAKE_INSTALL_PREFIX}/lib
    DOC "TEXTIO library.")

FIND_PATH(TEXTIO_INCLUDE_DIR 
    NAMES textio.h
    HINTS ${TEXTIO_HINT}/include ${CMAKE_INSTALL_PREFIX}/include
    DOC "TEXTIO include directory.")

set(TEXTIO_LIBRARIES ${TEXTIO_LIBRARY} )
set(TEXTIO_INCLUDE_DIRS ${TEXTIO_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set TEXTIO_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(TEXTIO DEFAULT_MSG
                                  TEXTIO_LIBRARY TEXTIO_INCLUDE_DIR)

mark_as_advanced(TEXTIO_INCLUDE_DIR TEXTIO_LIBRARY)

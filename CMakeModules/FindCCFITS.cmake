# - Find CCFITS

# Defines the following variables:
# CCFITS_INCLUDE_DIRS    - Location of CCFITS's include directory.
# CCFITS_LIBRARIES       - Location of CCFITS's libraries
# CCFITS_FOUND           - True if CCFITS has been located

find_package(CFITSIO REQUIRED)

FIND_LIBRARY(CCFITS_LIBRARY 
    NAMES libCCfits.so libCCfits.a
    DOC "CCFITS library.")

FIND_PATH(CCFITS_INCLUDE_DIR 
    NAMES CCfits.h
    HINTS /usr/include/CCfits
    DOC "CCOIFITS include directory.")

set(CCFITS_LIBRARIES ${CCFITS_LIBRARY} ${CFITSIO_LIBRARY})
set(CCFITS_INCLUDE_DIRS ${CCFITS_INCLUDE_DIR} ${CFITSIO_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CCOIFITS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CCFITS DEFAULT_MSG
                                  CCFITS_LIBRARY CCFITS_INCLUDE_DIR)

mark_as_advanced(CCFITS_INCLUDE_DIR CCFITS_LIBRARY)

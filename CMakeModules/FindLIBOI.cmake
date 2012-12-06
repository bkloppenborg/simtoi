# - Try to find the OpenCL Interferometry Library (liboi)
# Once done this will define
#  LIBOI_FOUND - System has LIBOI
#  LIBOI_INCLUDE_DIRS - The LIBOI include directories
#  LIBOI_LIBRARIES - The libraries needed to use LIBOI (none)

FIND_PACKAGE(OpenCL REQUIRED)
FIND_PACKAGE(OpenGL)
FIND_PACKAGE(CCOIFITS REQUIRED)
FIND_PACKAGE(TEXTIO REQUIRED)

FIND_LIBRARY(LIBOI_LIBRARY 
    NAMES oi
    HINTS ${LIBOI_HINT}/lib ${CMAKE_INSTALL_PREFIX}/lib
    DOC "LIBOI library.")

FIND_PATH(LIBOI_INCLUDE_DIR 
    NAMES oi.hpp
    HINTS ${LIBOI_HINT}/include ${CMAKE_INSTALL_PREFIX}/include
    DOC "LIBOI include directory.")

set(LIBOI_LIBRARIES ${LIBOI_LIBRARY} ${OPENCL_LIBRARIES} ${OPENGL_LIBRARIES} ${CCOIFITS_LIBRARIES} ${TEXTIO_LIBRARIES})
set(LIBOI_INCLUDE_DIRS ${LIBOI_INCLUDE_DIR} ${OPENCL_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${CCOIFITS_INCLUDE_DIRS} ${TEXTIO_INCLUDE_DIRS})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBOI_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBOI DEFAULT_MSG
                                  LIBOI_LIBRARY LIBOI_INCLUDE_DIR)

mark_as_advanced(LIBOI_INCLUDE_DIR LIBOI_LIBRARY)

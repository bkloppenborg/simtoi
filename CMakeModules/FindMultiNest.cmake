# - Find MultiNest
# Find the native MULTINEST includes and library
#
#  MULTINEST_INCLUDES    - where to find multinest.h
#  MULTINEST_LIBRARIES   - List of libraries when using MultiNest.
#  MULTINEST_FOUND       - True if MultiNest found.

if (MULTINEST_INCLUDES)
  # Already in cache, be silent
  set (MULTINEST_FIND_QUIETLY TRUE)
endif (MULTINEST_INCLUDES)

find_path (MULTINEST_INCLUDES includes/multinest.h)

find_library (MULTINEST_LIBRARIES NAMES multinest)

# handle the QUIETLY and REQUIRED arguments and set MULTINEST_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (MULTINEST DEFAULT_MSG MULTINEST_LIBRARIES MULTINEST_INCLUDES)

mark_as_advanced (MULTINEST_LIBRARIES MULTINEST_INCLUDES)

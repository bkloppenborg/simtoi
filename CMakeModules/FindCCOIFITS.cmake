# - Try to find CCOIFITS.
# Once executed, this module will define:
# Variables defined by this module:
#  CCOIFITS_FOUND        - system has CCOIFITS
#  CCOIFITS_INCLUDE_DIR  - the CCOIFITS include directory (cached)
#  CCOIFITS_INCLUDE_DIRS - the CCOIFITS include directories
#                         (identical to CCOIFITS_INCLUDE_DIR)
#  CCOIFITS_LIBRARY      - the CCOIFITS library (cached)
#  CCOIFITS_LIBRARIES    - the CCOIFITS libraries
#                         (identical to CCOIFITS_LIBRARY)
# 
# This module will use the following enviornmental variable
# when searching for CCOIFITS:
#  CCOIFITS_ROOT_DIR     - CCOIFITS root directory
#

# 
#  Copyright (c) 2012 Brian Kloppenborg
# 
#  This file is part of the C++ OIFITS Library (CCOIFITS).
#  
#  CCOIFITS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License 
#  as published by the Free Software Foundation, either version 3 
#  of the License, or (at your option) any later version.
#  
#  CCOIFITS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU Lesser General Public 
#  License along with CCOIFITS.  If not, see <http://www.gnu.org/licenses/>.
# 

if(NOT CCOIFITS_FOUND)

    find_path(CCOIFITS_INCLUDE_DIR 
        NAMES ccoifits.hpp
        HINTS $ENV{CCOIFITS_ROOT_DIR} 
        PATH_SUFFIXES include include/CCOIFITS
        DOC "CCOIFITS include directory.")
        
    find_library(CCOIFITS_LIBRARY 
        NAMES ccoifits
        HINTS $ENV{CCOIFITS_ROOT_DIR} 
        PATH_SUFFIXES lib
        DOC "CCOIFITS library.")
        
    find_package(CCFITS REQUIRED)
  
    mark_as_advanced(CCOIFITS_INCLUDE_DIR CCOIFITS_LIBRARY CCFITS_INCLUDE_DIR CCFITS_LIBRARY)

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(CCOIFITS DEFAULT_MSG
          CCOIFITS_LIBRARY CCOIFITS_INCLUDE_DIR)

    set(CCOIFITS_INCLUDE_DIRS ${CCOIFITS_INCLUDE_DIR} ${CCFITS_INCLUDE_DIR})
    set(CCOIFITS_LIBRARIES ${CCOIFITS_LIBRARY} ${CCFITS_LIBRARY})

endif(NOT CCOIFITS_FOUND)

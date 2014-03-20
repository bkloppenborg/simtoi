#
# Copyright (c) 2014 Brian Kloppenborg
#  
#  LIBOI is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License 
#  as published by the Free Software Foundation, either version 3 
#  of the License, or (at your option) any later version.
#  
#  LIBOI is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU Lesser General Public 
#  License along with LIBOI.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8)
project(chealpix)

find_package(CFITSIO REQUIRED)
include_directories(${CFITSIO_INCLUDE_DIRS})

# Find the source file, remove tests from library.
file(GLOB SOURCE *.c)
file(GLOB TESTS test_*.c)
list(REMOVE_ITEM SOURCE main.cpp ${TESTS})

# create the c-based healpix library.
add_library(chealpix SHARED ${SOURCE})
target_link_libraries(chealpix ${CFITSIO_LIBRARIES})

# Specify installation locations
install(TARGETS chealpix DESTINATION lib)
install(FILES chealpix.h DESTINATION include)

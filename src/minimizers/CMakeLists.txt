cmake_minimum_required(VERSION 2.8) 
project(simtoi_minimizers CXX)

# Include the previous directory to insure we get CMinimizer.h
INCLUDE(../)

# Assemble all of the model source
file(GLOB SOURCE *.cpp)

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
    file(GLOB REMOVE_MULTINEST "minimizers/CMultiNest.cpp")
    list(REMOVE_ITEM SOURCE ${REMOVE_MULTINEST})
endif(MULTINEST_FOUND)

# Now add the library
add_library(simtoi_minimizers ${SOURCE})
target_link_libraries(simtoi_minimizers ${LAPACK_LIBRARIES} ${OPTIONAL_LIBS})

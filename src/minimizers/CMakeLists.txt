cmake_minimum_required(VERSION 2.8) 
project(simtoi_minimizers CXX)

# Assemble all of the model source
file(GLOB SOURCE *.cpp)

# Levmar is built iff lapack (and transcendentally blas) are installed on the
# system.
if(LAPACK_FOUND)
    # Levmar does not export a LEVMAR_LIBRARIES variable, so we have to do it
    # ourselves. 
    LIST(APPEND OPTIONAL_LIBS levmar ${LAPACK_LIBRARIES})
    set(LEVMAR_DEF -D_ADD_LEVMAR)
    
    message(STATUS "LAPACK and BLAS found")
    message(STATUS " SIMTOI will include the Levmar minimizer.")
else(LAPACK_FOUND)    
    file(GLOB REMOVE_LEVMAR "CLevmar.cpp" "CBootstrap_Levmar.cpp")
    list(REMOVE_ITEM SOURCE ${REMOVE_LEVMAR})
    
    message(STATUS "LAPACK and BLAS NOT found")
    message(STATUS " SIMTOI will NOT include the Levmar minimizer.")
endif(LAPACK_FOUND)

# See if MultiNest is installed on the system. If so, include it.
find_package(MultiNest)
if(MULTINEST_FOUND)
    include_directories(${MULTINEST_INCLUDE_DIRS})
    LIST(APPEND OPTIONAL_LIBS ${MULTINEST_LIBRARIES})
    set(MULTINEST_DEF -D_ADD_MULTINEST)
    
    message(STATUS "MultiNest found")
    message(STATUS " SIMTOI will include the MultiNest minimizer.")
else(MULTINEST_FOUND)
    file(GLOB REMOVE_MULTINEST "CMultiNest.cpp")
    list(REMOVE_ITEM SOURCE ${REMOVE_MULTINEST})
    message(STATUS "MultiNest NOT found")
    message(STATUS " SIMTOI will NOT include the MultiNest minimizer.")
    
endif(MULTINEST_FOUND)

# Check for NLOpt, include it if found
find_package(NLopt)
if(NLOPT_FOUND)
    include_directories(${NLOPT_INCLUDE_DIRS})
    LIST(APPEND OPTIONAL_LIBS ${NLOPT_LIBRARIES})
    set(NLOPT_DEF -D_ADD_NLOPT)
    message(STATUS "NLopt found")
    message(STATUS " SIMTOI will include the NLopt minimizers.")
else(NLOPT_FOUND)
    file(GLOB REMOVE_NLOPT "CNLopt.cpp")
    list(REMOVE_ITEM SOURCE ${REMOVE_NLOPT})
    message(STATUS "NLopt NOT found")
    message(STATUS " SIMTOI will NOT include the NLopt minimizers.")
endif(NLOPT_FOUND)

# Now add the library
add_library(simtoi_minimizers ${SOURCE})
target_link_libraries(simtoi_minimizers ${OPTIONAL_LIBS})

set(MINIMIZER_INCLUDE_DIRS ${MULTINEST_INCLUDE_DIRS} ${NLOPT_INCLUDE_DIRS} PARENT_SCOPE)
set(MINIMISER_DEFINITIONS ${LEVMAR_DEF} ${MULTINEST_DEF} ${NLOPT_DEF} PARENT_SCOPE)

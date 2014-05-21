cmake_minimum_required(VERSION 2.8) 
project(simtoi_features CXX)

# Include the previous directory to insure we get CFeature.h
INCLUDE(../)

# Assemble all of the model source
file(GLOB SOURCE *.cpp)

# Now add the library
add_library(simtoi_features ${SOURCE})
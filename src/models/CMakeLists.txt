cmake_minimum_required(VERSION 2.8) 
project(simtoi_models CXX)

# Include the previous directory to insure we get CModel.h
INCLUDE(../)

# Assemble all of the model source
#file(GLOB SOURCE *.cpp)
file(GLOB SOURCE CSphere.*)

#file(GLOB SOURCE CSphere.* CDensityDisk.* CDisk_Pascucci2004.* 
#CDisk_Andrews2009.* CCylinder.* CDisk_ConcentricRings.* 
#CHealpixSpheroid.* CRocheBinary.* CRocheRotator.*)

# Now add the library
add_library(simtoi_models ${SOURCE})
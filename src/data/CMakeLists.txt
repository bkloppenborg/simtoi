cmake_minimum_required(VERSION 2.8) 
project(simtoi_data_support CXX)

# Include the previous directory to insure we get CData.h, CDataInfo.h and CDataHandler.h
INCLUDE(../)
# Include the QT directory to ensure we get CWorkerThread.h
INCLUDE(../QT)

# Assemble all of the model source
file(GLOB SOURCE *.cpp)

# Now add the library
add_library(simtoi_data_support ${SOURCE})
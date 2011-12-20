cmake_minimum_required(VERSION 2.8) 
project(threads CXX)

# MOC the headers:
file(GLOB HEADERS *.h)
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
file(GLOB SOURCE *.cpp)

add_library(threads ${SOURCE} ${HEADERS_MOC})
target_link_libraries(threads ${QT_LIBRARIES})

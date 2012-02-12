cmake_minimum_required(VERSION 2.8)
project(jsoncpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Call CMake on the subdirectories
add_subdirectory(src/lib_json)

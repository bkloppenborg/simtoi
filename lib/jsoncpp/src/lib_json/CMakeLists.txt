cmake_minimum_required(VERSION 2.8)
project(jsoncpp)

file(GLOB SOURCE *.cpp)
add_library(jsoncpp ${SOURCE})

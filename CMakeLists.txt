cmake_minimum_required(VERSION 2.8)

project(sostream_impls CXX)

include_directories(include)

set(CMAKE_CXX_STANDARD 11)

add_library(${CMAKE_PROJECT_NAME} tcpimpl.cpp)
cmake_minimum_required(VERSION 3.2)
project(sostream CXX)

set(CMAKE_CXX_STANDARD 11)
include_directories(include)

add_library(sostream prot_impl.cpp)
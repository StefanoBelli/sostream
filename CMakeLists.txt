cmake_minimum_required(VERSION 2.8)

project(sostream_impls CXX)

include_directories(include)

set(CMAKE_CXX_STANDARD 11)

set(PROJ_SOURCES tcpimpl.cpp)

if(WIN32)
	set(PROJ_SOURCES ${PROJ_SOURCES} wsiimpl.cpp)
endif()

add_library(${CMAKE_PROJECT_NAME} ${PROJ_SOURCES})
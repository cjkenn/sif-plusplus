cmake_minimum_required(VERSION 3.30.2)
project(sif)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SIF_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${SIF_SOURCE_DIR}/lib)
include_directories(BEFORE ${SIF_SOURCE_DIR}/include)

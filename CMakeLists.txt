cmake_minimum_required(VERSION 3.30.2)
project(sif VERSION 1.0)

set(CMAKE_C_COMPILER "/usr/bin/clang-10")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++-10")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SIF_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${SIF_SOURCE_DIR}/include)
add_subdirectory(${SIF_SOURCE_DIR}/lib)

add_executable(sif sif.cpp)

target_link_libraries(sif
  PUBLIC
  Driver
  Parser
)

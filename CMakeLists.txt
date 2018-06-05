cmake_minimum_required(VERSION 3.7)

project(mario)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_COMPILER "g++")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(mario/base)
add_subdirectory(mario/net)

cmake_minimum_required(VERSION 3.10)

project("Huffman Coding Root" CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BINARY_DIR        ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH     ${CMAKE_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

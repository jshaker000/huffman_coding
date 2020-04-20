cmake_minimum_required(VERSION 3.10)

project("Huffman Coding" CXX)

add_compile_options(-Wall -Werror -pedantic -Wextra -O3 -flto)

add_library(huffman_decode_tree huffman_decode_tree.cpp)
add_library(huffman_encode_tree huffman_encode_tree.cpp)
add_library(print_functions     print_functions.cpp)
add_library(bit_writer          bit_writer.cpp)

add_executable(huffman_encode huffman_encode.cpp)
add_executable(huffman_decode huffman_decode.cpp)
add_executable(huffman_codes  huffman_codes.cpp)

target_link_libraries(huffman_codes  huffman_encode_tree print_functions)
target_link_libraries(huffman_encode huffman_encode_tree bit_writer)
target_link_libraries(huffman_decode huffman_decode_tree)

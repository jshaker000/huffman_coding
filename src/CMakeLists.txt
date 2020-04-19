cmake_minimum_required(VERSION 3.10)

project("Huffman Coding")

add_compile_options(-Wall -Werror -pedantic -Wextra -O3 -flto)

add_library(huffman_decode_tree huffman_decode_tree.cpp ${HEADER_FILES})
add_library(huffman_encode_tree huffman_encode_tree.cpp ${HEADER_FILES})
add_library(print_functions     print_functions.cpp     ${HEADER_FILES})
add_library(bit_writer          bit_writer.cpp          ${HEADER_FILES})

add_executable(huffman_encode huffman_encode.cpp ${HEADER_FILES})
add_executable(huffman_decode huffman_decode.cpp ${HEADER_FILES})
add_executable(huffman_codes  huffman_codes.cpp  ${HEADER_FILES})

target_link_libraries(huffman_codes  huffman_encode_tree print_functions)
target_link_libraries(huffman_encode huffman_encode_tree bit_writer)
target_link_libraries(huffman_decode huffman_decode_tree)

# Huffman Coding

## Purpose

Huffman Coding is a suite of packages that work with
the Huffman Encoding algorithm to map out, compress,
and decompress files.

### Theory of Operation

Huffman Encoding is based on the idea that more
frequently occuring symbols can be represented with 
less than 8 bits to compress the data, and less 
frequently occuring symbols may take more than 8 bits

Huffman Encoding works best on files that have a skewed
distribution of bits. It is not as efficient on fairly uniform
data, like wav files or already encrypted / compressed data.

Huffman Encoding also places a header infront of the encoded
data. If the file is small enough, the gained compression
may not be worth the cost of the header.

For more Information [Wikipedia] (https://en.wikipedia.org/wiki/Huffman_coding)

## Contents
This program comes with 3 executables:

*huffman\_codes*
generates and prints the huffman codes
of the symbols from stdin or from a file
to stdout or a file in a nice ASCII format.

*huffman\_enocode*
compresses data from an infile to stdout
or a file using huffman coding

*huffman\_decode*
decompress data from stdin or an infile
compressed with huffman\_encode to
stdout or a file

### Building
To build, run

    mkdir build && cd build && cmake .. && make

The executable will be in the *build/* dir.

# Bugs And Future Work

-> allow huffman\_encode to read from a stream in
-> Figure out how to handle tree depths >= 64
-> Develop More Thorough Regression Suite
-> perhaps merge 3 executables into one with flags

** UPDATES MAY CHANGE THE FILE FORMAT, THIS IS STILL IN BETA
BE CAREFUL AND READ UPDATES TO THE README AND ROLL BACK
IF YOU NEED TO

If you find any other bugs or have any suggestions, please
send them to jshakes213@gmail.com or open an issue on git

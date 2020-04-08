// FILE FORMAT
//
// 2 BYTES - NUM DISTINCT SYMBOLS ENCODED
//
// FOR EACH SYMBOL:
    // 1 BYTE  - THE SYMBOL
    // 1 BYTE  - LENGTH OF THE HUFFMAN ENCODED SYMBOL IN BITS
    // N BYTES - THE HUFFMAN CODE (based on num bits. LSBS first)
// ENDOCDED DATA
// LAST BYTE HOLDS A TAG 1 - 8, saying how many bits of the second to last byte is encoded data
    // vs a pad

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <utility>
#include <unordered_map>

#include <unistd.h>

#include "huffman_encode_tree.h"
#include "bit_writer.h"

int main (int argc, char* argv[])
{

    std::string in;
    std::string out;

    if( isatty( STDIN_FILENO ) == 0 )
    {
        std::cerr << "ENCODE: Input must currently be from a file :(" << std::endl;
        return 1;
        // in = "/dev/stdin";
        // out = ( argc == 1 ? "/dev/stdout" : argv[ 1 ] );
    }

    else if (argc == 2 || argc == 3)
    {
        in  =   argv[1];
        out = ( argc == 2 ? "/dev/stdout" : argv[ 2 ] );
    }
    else
    {
        std::cerr << "ENCODE: Synatx: huffman_encode <infile> <outfile>" << std::endl;
        return 1;
    }

    std::fstream in_f(in, std::ios::binary | std::ios::in);

    if (in_f.fail())
    {
        std::cerr << "ENCODE: Error opening " << in << std::endl;
        return 2;
    }

    constexpr int in_buffsize = 1024*1024;
    std::unique_ptr<char[]> in_buffer(new char [in_buffsize]);

    //stores ASCII_CODE, FREQUENCY
    std::array<std::pair <char, std::uint64_t>,256> frequencies;

    for (int i = 0; i < frequencies.size(); i++)
    {
        frequencies[i].first  = i;
        frequencies[i].second = 0;
    }

    //do a first pass through the file counting frequencies
    while (in_f)
    {
        in_f.read (in_buffer.get(), in_buffsize);
        for (int i = 0; i < in_f.gcount(); i++)
        {
            int index = in_buffer.get()[i] >= 0 ? in_buffer.get()[i] : in_buffer.get()[i] + 0x0100;
            frequencies[index].second += 1;
        }
    }

    //sort by frequency
    std::sort(frequencies.begin(),
              frequencies.end(),
              [](const std::pair<char, std::uint64_t> &a, const std::pair<char,std::uint64_t> &b)
              { return a.second > b.second; }
             );

    // number the non zero frequencies
    int num_unique_chars = 0;

    for (int i = 0; i < frequencies.size() && frequencies[i].second != 0; i++)
    {
        num_unique_chars++;
    }

    if (num_unique_chars == 0)
    {
        std::cerr << "ENCODE: In file appears to be empty. Exiting" << std::endl;
        return 3;
    }

    // fill unordered map
    std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> huffman_map;
    {
        huffman::huffman_encode_tree enocde_tree(frequencies);
        enocde_tree.fill_unordered_map(huffman_map);
    }

    std::fstream out_f(out, std::ios::binary | std::ios::out);
    if (out_f.fail())
    {
        std::cerr << "ENCODE: Error opening " << out << std::endl;
        return 4;
    }

    std::uint64_t old_length_bits = 0;
    std::uint64_t new_length_bits = 0;

    // write the number of symbol codes to the file
    out_f.put(static_cast<char>((num_unique_chars >> 8) & 0xFF));
    out_f.put(static_cast<char>((num_unique_chars >> 0) & 0xFF));
    new_length_bits += 2*8;

    // print symbol codes to the file
    // SYMBOL[1 byte]  LENGTH[1 byte]  CODE[8 bytes]
    for (int i = 0; i < num_unique_chars; i++)
    {
        std::uint8_t  symbol     = frequencies[i].first;
        std::uint8_t  symbol_len = huffman_map[(frequencies[i].first)].first;
        std::uint64_t symbol_enc = huffman_map[(frequencies[i].first)].second;
        out_f.put(static_cast<char>(symbol));
        out_f.put(static_cast<char>(symbol_len));

        int bytes_to_print = symbol_len%8 == 0 ? symbol_len/8 : symbol_len/8  + 1;
        // print the symbol, LSBS first
        for (int j = 0; j < bytes_to_print; j++)
        {
             out_f.put(static_cast<char>(symbol_enc & 0xFF));
             symbol_enc >>= 8;
        }
        new_length_bits += 8*(1+1+bytes_to_print); // symbol, length, encoding
    }

    out_f.close();
    in_f.clear();
    in_f.seekg(std::ios_base::beg);
    {
        huffman::bit_writer b(out);

        //stream data and convert bits using lookup map
        //bit writer queues to write until a multiple of 8 bits is recieved
        while (in_f)
        {
            in_f.read (in_buffer.get(), in_buffsize);
            for (int i = 0; i < in_f.gcount(); i++)
            {
                b.add_bits(huffman_map[(in_buffer.get()[i])].first, huffman_map[(in_buffer.get()[i])].second);
                new_length_bits += huffman_map[(in_buffer.get()[i])].first;
                old_length_bits += 8;
            }
        }
        new_length_bits += 8; // tag byte
    }

    in_f.close();

    std::cerr << "ENCODE: DEFALTION RATE: " << std::setprecision(4)
              << static_cast<float>(100*new_length_bits)/old_length_bits << '%' << std::endl;

    return 0;

}

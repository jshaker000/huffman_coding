// FILE FORMAT
//
// 2 BYTES - NUM DISTINCT SYMBOLS ENCODED
//
// FOR EACH SYMBOL:
    // 1 BYTE  - THE SYMBOL
    // 1 BYTE  - LENGTH OF THE HUFFMAN ENCODED SYMBOL IN BITS
    // N BYTES - THE HUFFMAN CODE (based on num bits. LSBS first)
//
// ENDOCDED DATA
//
// LAST BYTE HOLDS A TAG 1 - 8, saying how many bits of the second to last byte is encoded data 
    // vs a pad

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <unistd.h>

#include "huffman_decode_tree.h"

// read in one bit at a time from the byte, turn left or right in the tree as appropriate
// If we hit a leaf, write it to the file and restart traversing the tree from the root for the
// next sequence.
static void traverse_tree_write(std::uint8_t num_bits_to_read,
                                std::uint8_t byte_to_read,
                                huffman::huffman_decode_tree &decode_tree,
                                char *out_buffer,
                                std::uint64_t out_buffsize,
                                std::uint64_t &out_buff_bytes,
                                std::fstream  &out_f)
{
    for (int j = 0; j < num_bits_to_read; j++)
    {
        huffman::huffman_decode_tree::Direction d = ((byte_to_read >> (7-j)) & 0x01) == 0x01 ? huffman::huffman_decode_tree::Direction::RIGHT :
                                                    huffman::huffman_decode_tree::Direction::LEFT;
        struct huffman::huffman_decode_tree::decode_status traverse_result = decode_tree.move_direction(d);
        if (traverse_result.is_valid && traverse_result.is_leaf)
        {
            out_buffer[out_buff_bytes] = traverse_result.symbol;
            out_buff_bytes += 1;
            if (out_buff_bytes == out_buffsize)
            {
                out_f.write(out_buffer, out_buffsize);
                out_buff_bytes = 0;
            }
            decode_tree.move_direction(huffman::huffman_decode_tree::Direction::RESET);
        }
        else if (!traverse_result.is_valid)
        {
            std::cerr << "Huffman Decode: Error Parsing File (traversing symbol tree)" << std::endl;
            std::exit(-1);
        }
    }
}

int main (int argc, char* argv[])
{
    std::string in;
    std::string out;

    if(isatty(STDIN_FILENO) == 0)
    {
        in = "/dev/stdin";
        out = argc == 1 ? "/dev/stdout" : argv[1];
    }

    else if (argc == 2 || argc == 3)
    {
        in  = argv[1];
        out = argc == 2 ? "/dev/stdout" : argv[2];
    }
    else
    {
        std::cerr << "DECODE: Synatx: huffman_decode <infile> <outfile>" << std::endl;
        return 1;
    }

    std::fstream in_f(in, std::ios::binary | std::ios::in);

    if (in_f.fail())
    {
        std::cerr << "DECODE: Error opening " << in << std::endl;
        return 2;
    }
    // bug if this equals 1 (and its slower anyway so don't do that)
    constexpr std::uint64_t in_buffsize  = 256*1024;
    std::unique_ptr<char[]> in_buffer(new char[in_buffsize]);

    // read in the number of symbols
    int num_codes;
    {
        char byte_0, byte_1;
        in_f.get(byte_0);
        in_f.get(byte_1);
        num_codes = (static_cast<unsigned int>(byte_0) * 0x0100) + static_cast<unsigned int>(byte_1);
    }

    std::vector <struct huffman::huffman_decode_tree::symbol_len_encode> huffman_nodes;
    huffman_nodes.reserve(num_codes);

    // iterate through each symbol table in header and build the list of nodes
    // symbol, length, byte code
    for (int i=0; i < num_codes; i++)
    {
        char symbol  = 0;
        char enc_len = 0;
        std::uint64_t byte_code = 0;

        in_f.get(symbol);
        in_f.get(enc_len);

        // read data back, LSBs first
        int bytes_to_read = enc_len%8 == 0 ? enc_len/8 : enc_len/8 + 1;
        for (int i = 0; i < bytes_to_read; i++)
        {
            char tmp;
            in_f.get(tmp);
            byte_code += static_cast<std::uint8_t>(tmp) << (8*i);
        }
        struct huffman::huffman_decode_tree::symbol_len_encode sle{ symbol, static_cast<std::uint8_t>(enc_len), byte_code};
        huffman_nodes.push_back(std::move(sle));
    }

    std::fstream out_f(out, std::ios::binary | std::ios::out);
    if (out_f.fail())
    {
        std::cerr << "DECODE: Error opening " << out << std::endl;
        return 4;
    }

    constexpr std::uint64_t out_buffsize  = 256*1024;
    std::uint64_t out_buff_bytes          = 0;
    std::unique_ptr<char[]> out_buffer(new char[out_buffsize]);


    huffman::huffman_decode_tree decode_tree(huffman_nodes);

    char second_to_last_byte_read = '\0';
    char last_byte_read           = '\0';
    bool first_time               = true;
    // extract the next bit and traverse the tree until we hit a leaf
    // write to a file
    // the last 2 bytes in the file must be treated differently as they have the last code and the num bits of the
    // last byte to read
    while (in_f)
    {
        in_f.read (in_buffer.get(), in_buffsize);
        if (in_f.gcount() == 1)
        {
            if (first_time)
            {
                std::cerr << "Decode: Error Parsing File!" << std::endl;
                return 5;
            }
            second_to_last_byte_read = last_byte_read;
            last_byte_read           = in_buffer.get()[0];
        }
        else
        {
            if (!first_time)
            {
                traverse_tree_write(8,
                                    static_cast<std::uint8_t>(last_byte_read),
                                    decode_tree,
                                    out_buffer.get(),
                                    out_buffsize,
                                    out_buff_bytes,
                                    out_f);
            }
            second_to_last_byte_read = in_buffer.get()[in_f.gcount()-2];
            last_byte_read           = in_buffer.get()[in_f.gcount()-1];
        }
        // loop all bytes read except for the last 2 iff they are the last 2 in the file
        for (int i = 0; (in_f && i < in_f.gcount() - 1) || (!in_f && i < in_f.gcount() - 2); i++)
        {
            traverse_tree_write(8,
                                static_cast<std::uint8_t>(in_buffer.get()[i]),
                                decode_tree,
                                out_buffer.get(),
                                out_buffsize,
                                out_buff_bytes,
                                out_f);
        }
        // read in only as many bits from the second to last byte as stated in the last byte
        if (!in_f)
        {
            traverse_tree_write(static_cast<std::uint8_t>(last_byte_read),
                                static_cast<std::uint8_t>(second_to_last_byte_read),
                                decode_tree,
                                out_buffer.get(),
                                out_buffsize,
                                out_buff_bytes,
                                out_f);
        }
        first_time = false;
    }
    out_f.write(out_buffer.get(), out_buff_bytes);
    return 0;
}

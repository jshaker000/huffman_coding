// FILE FORMAT
//
// 2 BYTES - NUM DISTINCT SYMBOLS ENCODED
//
// FOR EACH SYMBOL:
    // 1 BYTE  - THE SYMBOL
    // 1 BYTE  - LENGTH OF THE HUFFMAN ENCODED SYMBOL IN BITS
    // 8 BYTES - THE HUFFMAN CODE
//
// ENDOCDED DATA
//
// LAST BYTE HOLDS A TAG 1 - 8, saying how many bits of the second to last byte is encoded data 
    // vs a pad

#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>

#include <unistd.h>

#include "ancillary_functions.h"

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

    if ( in_f.fail() )
    {
        std::cerr << "DECODE: Error opening " << in << std::endl;
        return 2;
    }

    constexpr int in_buffsize  = 256*1024;
    char in_buffer[in_buffsize] = {}; 
    std::unordered_map<std::pair<std::uint8_t, std::uint64_t>, char> huffman_map;

    // read in the number of symbols
    int num_codes;
    {
        char byte_0, byte_1;
        in_f.get(byte_0);
        in_f.get(byte_1);
        num_codes = (static_cast<unsigned int>(byte_0) * 0x0100) + static_cast<unsigned int>(byte_1);
    }

    // iterate through each symbol table in header and build the unordered map
    // pair <length, code(binary)>, ASCII SYMBOL
    for (int i=0; i < num_codes; i++)
    {
        char symbol  = 0;
        char enc_len = 0;
        std::uint64_t byte_code = 0;

        in_f.get(symbol);
        in_f.get(enc_len);

        // read data back, LSBs first
        int bytes_to_read = enc_len%8 == 0 ? enc_len/8 : enc_len/8 + 1;
        std::uint64_t pow256 = 0x01;
        for (int i = 0; i < bytes_to_read; i++)
        {
            char tmp;
            in_f.get(tmp);
            byte_code += static_cast<std::uint8_t>(tmp) * pow256;
            pow256    *= 0x0100;
        }
        if (!huffman_map.emplace(std::pair<std::uint8_t, std::uint64_t>(static_cast<std::uint8_t>(enc_len), byte_code), symbol).second)
        {
            std::cerr << "DECODE: key collision building hashtable. error" << std::endl;
            return 3;
        }
    }

    std::fstream out_f(out, std::ios::binary | std::ios::out);
    if (out_f.fail())
    {
        std::cerr << "DECODE: Error opening " << out << std::endl;
        return 4;
    }

    std::pair          <std::uint8_t, std::uint64_t> key(0,0);
    std::unordered_map <std::pair<std::uint8_t, std::uint64_t>, char>::const_iterator found;

    // extract the next bit into 'current code'
    // appropriately checking the <length, binary code> pair until it matches a symbol in the map, then
    // writing to a file
    while (in_f)
    {
        in_f.read (in_buffer, in_buffsize);
        // loop all bytes read except for the last 2 iff they are the last 2 in the file
        for (int i = 0; (in_f && i < in_f.gcount()) || (!in_f && i < in_f.gcount() - 2); i++)
        {
            std::uint8_t pow2 = 0x0080;
            for (int j = 0; j < 8; j++)
            {
                key.first  += 1;
                key.second *= 2;
                key.second += ((static_cast<uint8_t>(in_buffer[i]) / pow2) & 0x01);
                found = huffman_map.find(key);
                if ((found != huffman_map.end()))
                {
                    out_f.put(found->second);
                    key.first  = 0;
                    key.second = 0;
                }
                pow2 /= 2;
            }
        }
        // read in only as many bits from the second to last byte as stated in the last byte
        if (!in_f)
        {
            std::uint8_t pow2 = 0x0080;
            for (int j = 0; j < in_buffer[in_f.gcount() - 1]; j++)
            {
                key.first  += 1;
                key.second *= 2; 
                key.second += ((static_cast<uint8_t>(in_buffer[in_f.gcount()-2]) / pow2) & 0x01);

                found = huffman_map.find(key);

                if ((found != huffman_map.end()))
                {
                    out_f.put(static_cast<char>(found->second));
                    key.first  = 0;
                    key.second = 0;
                }
                pow2 /= 2;
            }
        }
    }
    return 0;
}

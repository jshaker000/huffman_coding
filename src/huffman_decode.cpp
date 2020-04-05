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
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>

#include <unistd.h>

#include "ancillary_functions.h"

static void update_key_write(std::uint8_t num_bits_to_read,
                             std::uint8_t byte_to_read,
                             std::pair<std::uint8_t,std::uint64_t> &key,
                             std::unordered_map<std::pair<std::uint8_t, std::uint64_t>, char> &huffman_map,
                             char *out_buffer,
                             std::uint64_t out_buffsize,
                             std::uint64_t &out_buff_bytes,
                             std::fstream  &out_f)
{
    for (int j = 0; j < num_bits_to_read; j++)
    {
        key.first  += 1;
        key.second *= 2;
        key.second += (byte_to_read >> (7-j)) & 0x01;
        auto found = huffman_map.find(key);
        if ((found != huffman_map.end()))
        {
            out_buffer[out_buff_bytes] = found->second;
            out_buff_bytes += 1;
            if (out_buff_bytes == out_buffsize)
            {
                out_f.write(out_buffer, out_buffsize);
                out_buff_bytes = 0;
            }
            key.first  = 0;
            key.second = 0;
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

    if ( in_f.fail() )
    {
        std::cerr << "DECODE: Error opening " << in << std::endl;
        return 2;
    }
    // bug if this equals 1 (and its slower anyway so don't do that)
    constexpr std::uint64_t in_buffsize  = 256*1024;
    std::unique_ptr<char[]> in_buffer(new char[in_buffsize]);

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
        for (int i = 0; i < bytes_to_read; i++)
        {
            char tmp;
            in_f.get(tmp);
            byte_code += static_cast<std::uint8_t>(tmp) << (8*i);
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

    constexpr std::uint64_t out_buffsize  = 256*1024;
    std::uint64_t out_buff_bytes          = 0;
    std::unique_ptr<char[]> out_buffer(new char[out_buffsize]);

    std::pair <std::uint8_t, std::uint64_t> key(0,0);
    char second_to_last_byte_read;
    char last_byte_read;
    bool first_time = true;
    // extract the next bit into the key
    // appropriately checking the <length, binary code> pair until it matches a symbol in the map, then
    // write to a file
    // the last 2 bytes in the file must be treated differently as they have the last code and the num bits of the
    // last byte to read
    while (in_f)
    {
        in_f.read (in_buffer.get(), in_buffsize);
        if (in_f.gcount() == 1)
        {
            second_to_last_byte_read = last_byte_read;
            last_byte_read           = in_buffer.get()[0];
        }
        else
        {
            if (!first_time)
            {
                update_key_write(8,
                                 static_cast<std::uint8_t>(last_byte_read),
                                 key,
                                 huffman_map,
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
            update_key_write(8,
                             static_cast<std::uint8_t>(in_buffer.get()[i]),
                             key,
                             huffman_map,
                             out_buffer.get(),
                             out_buffsize,
                             out_buff_bytes,
                             out_f);
        }
        // read in only as many bits from the second to last byte as stated in the last byte
        if (!in_f)
        {
            update_key_write(static_cast<std::uint8_t>(last_byte_read),
                             static_cast<std::uint8_t>(second_to_last_byte_read),
                             key,
                             huffman_map,
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
// prints the huffman codes of a file in a nicely formatted table

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <unistd.h>

#include "huffman_encode_tree.h"
#include "print_functions.h"

int main (int argc, char* argv[])
{

    std::string in;
    std::string out;

    if( isatty(STDIN_FILENO) == 0 )
    {
        in = "/dev/stdin";
        out = argc == 1 ? "/dev/stdout" : argv[1];
    }

    else if (argc == 2 || argc == 3)
    {
        in = argv[1];
        out = argc == 2 ? "/dev/stdout" : argv[2];
    }
    else
    {
        std::cerr << "CODES: Synatx: huffman_codes <in file> <outfile>" << std::endl;
        return 1;
    }

    std::fstream in_f(in, std::ios::binary | std::ios::in);

    if ( in_f.fail() )
    {
        std::cerr << "CODES: Error opening " << in << std::endl;
        return 2;
    }

    constexpr int buffsize = 256*1024;
    std::unique_ptr<char[]> buffer(new char[buffsize]);

    std::array<std::pair <char, std::uint64_t>,256> frequencies;

    int num_unique_chars = 0;

    for (int i = 0; i < frequencies.size(); i++)
    {
        frequencies[i].first  = i;
        frequencies[i].second = 0;
    }

    while (in_f)
    {
        in_f.read (buffer.get(), buffsize);
        for (int i = 0; i < in_f.gcount(); i++)
        {
            int index = buffer.get()[i] >= 0 ? buffer.get()[i] : buffer.get()[i] + 0x0100;
            frequencies[index].second += 1;
        }
    }
    in_f.close();

    //sort by frequency
    std::sort(frequencies.begin(),
              frequencies.end(),
              [](const std::pair<char, std::uint64_t> &a, const std::pair<char,std::uint64_t> &b)
              { return a.second > b.second; }
             );

    // non zero frequencies
    for (int i = 0; i < frequencies.size() && frequencies[i].second != 0; i++)
    {
        num_unique_chars++;
    }

    if (num_unique_chars == 0)
    {
        std::cerr << "CODES: In file appears to be empty. Exiting" << std::endl;
        return 3;
    }

    // fill unordered map
    std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> huffman_map;
    {
        huffman::huffman_encode_tree encode_tree(frequencies);
        encode_tree.fill_unordered_map(huffman_map);
    }

    std::fstream out_f(out, std::ios::binary | std::ios::out);
    if (out_f.fail())
    {
        std::cerr << "CODES: Error opening " << out << std::endl;
        return 4;
    }

    std::uint64_t old_length_bits = 0;
    std::uint64_t new_length_bits = 0;

    out_f     << "FILE:               " << in               << '\n'
              << "NUM_UNIQUE_SYMBOLS: " << num_unique_chars << '\n'
              << "+-------------+-----------+----START-huffman-codes----+-------------------+" << '\n'
              << "|    ASCII    | Frequency |       HUFFMAN Code        | New Length (bits) |" << '\n'
              << "+-------------+-----------+---------------------------+-------------------+" << std::endl;

    for (int i = 0; i < frequencies.size(); i++)
    {
        if  (frequencies[ i ].second != 0)
        {
            std::string code = huffman::to_binary(static_cast<char>(huffman_map[frequencies[i].first].second),
                                                  huffman_map[frequencies[i].first].first);

            out_f     << "| " << std::setw(10) << huffman::printable_ascii(frequencies[i].first) << "  | "
                      << std::setw(9)  << frequencies[ i ].second << " | "
                      << std::setw(25) << code << " | "
                      << std::setw(17) << code.length() << " |" << std::endl;

            old_length_bits += 8 * frequencies[i].second ;
            new_length_bits += code.length() * frequencies[i].second;
        }
    }

    out_f     << "+-------------+-----------+-----END-huffman-codes-----+-------------------+" << '\n'
              << "Old legnth (bytes): " << std::setw( 12 ) << ( old_length_bits >> 3 ) << '\n'
              << "New length (bytes): " << std::setw( 12 ) << ( new_length_bits >> 3 ) << '\n'
              << "DEFALTION RATE:           " << std::setprecision( 4 ) 
              << static_cast<float>( 100 * new_length_bits ) / old_length_bits << '%' << std::endl;

    return 0;

}

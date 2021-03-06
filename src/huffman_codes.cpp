// prints the huffman codes of a file in a nicely formatted table
// This also provides an approx deflation rate, but it isnt extremely accurate
// as it doesnt include the size of the header table

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>

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
        std::cerr << "HUFMAN CODES: Synatx: huffman_codes <in file> <outfile>" << std::endl;
        return 1;
    }

    std::fstream in_f(in, std::ios::binary | std::ios::in);

    if (in_f.fail())
    {
        std::cerr << "HUFMAN CODES: Error opening " << in << std::endl;
        return 2;
    }

    constexpr int buffsize = 256*1024;
    std::unique_ptr<char[]> buffer(new char[buffsize]);

    //stores ASCII_CODE, FREQUENCY
    std::array<struct huffman::encode_tree::symb_freq,256> frequencies;

    for (size_t i = 0; i < frequencies.size(); i++)
    {
        frequencies[i].symbol    = static_cast<char>(i);
        frequencies[i].frequency = 0;
    }

    while (in_f)
    {
        in_f.read(buffer.get(), buffsize);
        std::for_each(buffer.get(), buffer.get()+in_f.gcount(),
        [&frequencies](auto c)
        {
            size_t index = c >= 0 ? c : c + 0x0100;
            frequencies[index].frequency += 1;
        });
    }
    in_f.close();

    //sort by frequency
    std::sort(frequencies.begin(),
              frequencies.end(),
              [](const auto &a, const auto &b)
              { return a.frequency > b.frequency; }
             );

    // non zero frequencies
    int num_unique_chars = 0;
    std::for_each(frequencies.begin(), frequencies.end(),
    [&num_unique_chars](const auto &f)
    {
        if (f.frequency != 0)
        {
            num_unique_chars++;
        }
    });

    if (num_unique_chars == 0)
    {
        std::cerr << "HUFMAN CODES: In file appears to be empty. Exiting" << std::endl;
        return 3;
    }

    // fill unordered map
    std::unordered_map<char, struct huffman::encode_tree::len_encode> huffman_map;
    {
        huffman::encode_tree encode_tree(frequencies);
        encode_tree.fill_unordered_map(huffman_map);
    }

    std::fstream out_f(out, std::ios::binary | std::ios::out);
    if (out_f.fail())
    {
        std::cerr << "HUFMAN CODES: Error opening " << out << std::endl;
        return 4;
    }

    out_f     << "FILE:               " << in               << '\n'
              << "NUM_UNIQUE_SYMBOLS: " << num_unique_chars << '\n'
              << "+-------------+-----------+----START-huffman-codes----+-------------------+" << '\n'
              << "|    ASCII    | Frequency |       HUFFMAN Code        | New Length (bits) |" << '\n'
              << "+-------------+-----------+---------------------------+-------------------+" << std::endl;

    std::uint64_t old_length_bits = 0;
    std::uint64_t new_length_bits = 0;

    std::for_each (frequencies.begin(), frequencies.begin()+num_unique_chars,
    [&out_f, &huffman_map, &old_length_bits, &new_length_bits](const auto & f)
    {
        const auto &enc = huffman_map[f.symbol];
        out_f     << "| " << std::setw(10) << huffman::printable_ascii(f.symbol) << "  | "
                  << std::setw(9)  << f.frequency << " | "
                  << std::setw(25) << huffman::to_binary(enc.encoding, enc.length) << " | "
                  << std::setw(17) << static_cast<int>(enc.length) << " |" << std::endl;

        old_length_bits += 8 * f.frequency;
        new_length_bits += enc.length * f.frequency;
    });

    out_f     << "+-------------+-----------+-----END-huffman-codes-----+-------------------+" << '\n'
              << "Old legnth (bytes): " << std::setw(12) << (old_length_bits >> 3) << '\n'
              << "New length (bytes): " << std::setw(12) << (new_length_bits >> 3) << '\n'
              << "DEFALTION RATE:           " << std::setprecision( 4 )
              << static_cast<double>(100 * new_length_bits) / old_length_bits << '%' << std::endl;

    return 0;

}

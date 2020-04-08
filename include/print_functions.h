#ifndef PRINT_FUNCTIONS
#define PRINT_FUNCTIONS

#include <cstdint>
#include <string>

// extra functions to make printing / debugging easier

namespace huffman
{
    std::string to_binary(std::uint64_t num, std::uint8_t bits);
    std::string printable_ascii(unsigned char c);
}

#endif

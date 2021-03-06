#ifndef BIT_WRITER
#define BIT_WRITER

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

// takes in a file name and appends to it bits.
// Allowing you insert non bytes at a time
// Flushes on deconstruction and adds a tag byte of how many bits in the second to last byte were valid
namespace huffman
{
    class bit_writer
    {
        public:
            bit_writer(const std::string &);
            ~bit_writer();
            void add_bits (std::uint8_t, std::uint64_t);
        private:
            static constexpr std::uint64_t buff_size = 256*1024;
            std::unique_ptr<char[]> buffer           = nullptr;
            std::uint64_t current_bits = 0;
            std::fstream out_f;
    };
}

#endif

#ifndef BIT_WRITER
#define BIT_WRITER

#include <cstdint>
#include <fstream>
#include <string>

// takes in a file name and appends to it bits.
// takes in chars, but only reads "bits" at a time. Flushes on deconstruction
class bit_writer
{
    public:
        bit_writer(const std::string &);
        ~bit_writer();
        void add_bits (std::uint8_t, std::uint64_t);
    private:
        static constexpr std::uint64_t buff_size = 256*1024;
        char buffer[buff_size] = {};
        std::uint64_t current_bits = 0;
        std::fstream out_f;
};

#endif

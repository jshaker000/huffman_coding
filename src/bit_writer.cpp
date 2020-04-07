#include "bit_writer.h"

bit_writer::bit_writer (const std::string &out)
{
    buffer = std::unique_ptr<char[]>(new char[buff_size]);
    buffer.get()[0] = '\0';
    out_f.open(out, std::ios::binary | std::ios::app);
}

// flush buffer
// write the number of bits in the last byte to read,
// and close file
bit_writer::~bit_writer()
{
    if (current_bits !=0)
        out_f.write(buffer.get(), (current_bits/8) + (current_bits%8 != 0 ? 1 : 0));
    out_f.put(static_cast<char>(current_bits%8 != 0 ? current_bits%8 : 8));
    out_f.close();
}

// adds the smallest bits_to_add of bits into the buffer and saves the new position
// flush buffer when needed
void bit_writer::add_bits(std::uint8_t bits_to_add, std::uint64_t bits)
{
    std::uint64_t index_to_add = current_bits >>    3;
    std::uint8_t  slot_to_add  = current_bits &  0x07;

    while (bits_to_add != 0)
    {
        buffer.get()[index_to_add] |= (bits & (1<<(bits_to_add - 1))) >> (bits_to_add - 1)
                                      << (7 - slot_to_add);
        slot_to_add++;

        if (slot_to_add == 8)
        {
            slot_to_add = 0;
            index_to_add++;
            if (index_to_add == buff_size)
            {
                out_f.write(buffer.get(), buff_size);
                index_to_add =  0;
                current_bits = -1;
            }
            buffer.get()[index_to_add] = '\0';
        }
        current_bits++;
        bits_to_add--;
    }
}

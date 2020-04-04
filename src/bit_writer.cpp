#include "bit_writer.h"

bit_writer::bit_writer (const std::string &out)
{
    out_f.open(out, std::ios::binary | std::ios::app);
}

// flush buffer
// write the number of bits in the last byte to read,
// and close file
bit_writer::~bit_writer()
{
    if ( current_bits !=0 )
        out_f.write(buffer, (current_bits + 8 - ((current_bits - 1) % 8)) >> 3);
    out_f.put(static_cast<char>((current_bits % 8) ? (current_bits % 8) : (8)));
    out_f.close();
}

// adds the smallest bits_to_add of bits into the buffer and saves the new position
// flush buffer when needed
void bit_writer::add_bits(char bits_to_add, std::uint64_t bits )
{
    int index_to_add = current_bits / 8;
    int slot_to_add  = current_bits % 8;

    while (bits_to_add > 0)
    {
        buffer[index_to_add] |= (bits & (1<<(bits_to_add - 1))) >>  (bits_to_add - 1)
                               << (7 - slot_to_add);
        slot_to_add++;

        if ( slot_to_add == 8 )
        {
            slot_to_add = 0;
            index_to_add++;

            if (index_to_add == buff_size)
            {
                out_f.write( buffer, buff_size );
                index_to_add = 0;
                current_bits = -1;
                for (int i = 0; i < buff_size; i++ )
                {
                    buffer[i] = 0;
                }
            }
        }
        current_bits++;
        bits_to_add--;
    }
}

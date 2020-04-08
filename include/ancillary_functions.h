#ifndef ANCILLARY_FUNCTIONS
#define ANCILLARY_FUNCTIONS

#include <cstdint>
#include <string>

namespace huffman
{
    template <class T>
    std::string to_binary(T num, std::uint8_t bits)
    {
        std::string s = "";
        // special arg, pad to nearest byte.
        if (bits == 0xFF)
        {
            while (num != 0)
            {
                s = static_cast<char>( (num&0x01) +'0') + s;
                num >>= 1;
            }

            while (s.length() % 8 != 0 || s.length() == 0)
            {
                s = "0" + s;
            }
        }
        else
        {
            for (std::uint8_t i = 0; i < bits; i++)
            {
                s = static_cast<char>((num & 0x01) + '0') + s;
                num >>= 1;
            }
        }
        return s;
    }

    std::string printable_ASCII (unsigned char c)
    {
        switch ( c )
        {
            // speical chars
            case '\a': return ( "\\a " );
            case '\b': return ( "\\b " );
            case '\n': return ( "\\n " );
            case '\r': return ( "\\r " );
            case '\t': return ( "\\t " ); 
            case '\v': return ( "\\v " );
            case ' ' : return ("\" \"");
            default:
                std::string s;
                // printable chars
                if (c >= 0x20 && c <=0x7E)
                {
                    s.push_back(c);
                    s += "  ";
                    return s;
                }
                // hex
                else
                {
                    char high  = ((c&0xF0) >> 4);
                    char low   = ((c&0x0F));
                    s += (high >= 0x0A ? high+'A'-0x0A : high+'0');
                    s += (low  >= 0x0A ? low +'A'-0x0A : low +'0');
                    return "0x" + s;
                }
        }
    }
}

#endif
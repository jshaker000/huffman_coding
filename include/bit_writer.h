#ifndef BIT_WRITER
#define BIT_WRITER

#include <fstream>
#include <string>

constexpr int buff_size = 256*1024;

class bit_writer
{
    public:
        bit_writer( std::string );
        ~bit_writer( );
        void add_bits ( char, long long int );
    private:
        char buffer[ buff_size ] = {};
        int current_bits = 0;
        std::fstream out_f;
};

#endif
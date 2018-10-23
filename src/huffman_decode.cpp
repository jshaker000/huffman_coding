//FILE FORMAT
//
//2 BYTES - NUM DISTINCT SYMBOLS ENCODED
//
//FOR EACH SYMBOL:
    //1 BYTE  - THE SYMBOL
    //1 BYTE  - LENGTH OF THE HUFFMAN ENCODED SYMBOL IN BITS
    //8 BYTES - THE HUFFMAN CODE
//
//ENDOCDED DATA
//
//LAST BYTE HOLDS A TAG 1 - 8, saying how many bits of the second to last byte is encoded data 
    //vs a pad

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>

#include <arpa/inet.h> //for htonl to check endianness
#include <unistd.h>

#include "ancillary_functions.h"

const bool IS_BIG_ENDIAN = ( htonl(47) == 47 );

int main ( int argc, char* argv[])
{

    std::string in;
    std::string out;

    if( isatty( STDIN_FILENO ) == 0 )
    {
        in = "/dev/stdin";
        out = ( argc == 1 ? "/dev/stdout" : argv[ 1 ] );
    }

    else if ( argc == 2 || argc == 3 )
    {
        in  =   argv[ 1 ];
        out = ( argc == 2 ? "/dev/stdout" : argv[ 2 ] );
    }
    else
    {
        std::cerr << "Synatx: huffman_decode <in file> <outfile>" << std::endl;
        return 1;
    }
    
    std::fstream in_f( in, std::ios::binary | std::ios::in );

    if ( in_f.fail() )
    {
        std::cerr << "Error opening " << in << std::endl;
        return 2;
    }

    constexpr int in_buffsize  = 256*1024;
    char in_buffer[ in_buffsize ] = {}; 
    std::unordered_map< std::pair<char, long long int>, char > huffman_map;

    //read in the number of symbols
    int num_codes;
    {
        char byte_0, byte_1;
        in_f.get(byte_0);
        in_f.get(byte_1);
        num_codes = ( static_cast<unsigned int>( byte_0 ) << 8 ) + static_cast<unsigned int>( byte_1 );
    }

    //iterate through each symbol table in header and build the unordered map
    //pair <length, code( binary )>, ASCII SYMBOL
    for ( int i = 0; i < num_codes; i++ )
    {
        char value = {};
        char length = {};
        char byte_code_buff [ 8 ] = {};
        long long int byte_code   = {};
        
        in_f.get(value);
        in_f.get(length);

        in_f.read(byte_code_buff, 8 );
        
        if ( !IS_BIG_ENDIAN )  //little endian, invert symbol order so conversion to long long int is correct
        {
            for ( int i = 0; i < 3; i++ )
            {
                char temp = byte_code_buff[ i ];
                byte_code_buff[ i ] = byte_code_buff[ 8 - 1 - i ];
                byte_code_buff[ 8 - 1 - i ] = temp;
            }
        }
        
        //turn array of chars into a long long int
        byte_code = reinterpret_cast<long long int *>( byte_code_buff  )[ 0 ];

        std::pair < char, long long int > p( length, byte_code );

        if ( !huffman_map.emplace( p, value ).second )
        {
            std::cerr << "key collision. error" << std::endl;
            return 3;
        }
    }

    std::fstream out_f( out, std::ios::binary | std::ios::out);
    if ( out_f.fail() )
    {
        std::cerr << "Error opening " << out << std::endl;
        return 4;
    }

    std::pair < char, long long int > key( 0,0 );
    std::unordered_map< std::pair<char, long long int>, char >::const_iterator found;
   
    //extract the next bit into 'current code'
    //appropriately checking the <length, binary code> pair until it matches a symbol in the map, then
    //writing to a file
    while ( in_f )
    {
        in_f.read ( in_buffer, in_buffsize );
        //loop all bytes read except for the last 2 iff they are the last 2 in the file
        for ( int i = 0; ( in_f && i < in_f.gcount() ) || ( !in_f && i < in_f.gcount() - 2 ); i++ )
        {
            for ( int j = 0; j < 8; j++ )
            {
                key.first  +=  1;
                key.second <<= 1; 
                key.second += ( ( in_buffer[ i ] >> ( 7 - j ) ) & 1 );

                found = huffman_map.find( key );
                
                if ( ( found != huffman_map.end() ) )
                {
                    out_f.put( found->second );
                    key.first  = 0;
                    key.second = 0;
                }
            }
        }
        //read in only as many bits from the second to last byte as stated in the last byte
        if ( !in_f )
        {
            for ( int j = 0; j < in_buffer[ in_f.gcount() - 1 ]; j++ )
            {
                key.first  +=  1;
                key.second <<= 1; 
                key.second += ( ( in_buffer[ in_f.gcount() - 2 ] >> ( 7 - j ) ) & 1 );

                found = huffman_map.find( key );
                    
                if ( ( found != huffman_map.end() ) )
                {
                    out_f.put( found->second );
                    key.first  = 0;
                    key.second = 0;
                }
            }
        }
    }

    return 0;

}
//prints the huffman codes of a file in a nicely formatted table

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <unistd.h>

#include "huffman_tree.h"
#include "ancillary_functions.h"

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
        std::cerr << "Synatx: huffman_codes <in file> <outfile>" << std::endl;
        return 1;
    }
    
    std::fstream in_f( in, std::ios::binary | std::ios::in );

    if ( in_f.fail() )
    {
        std::cerr << "Error opening " << in << std::endl;
        return 2;
    }

    constexpr int buffsize = 256*1024;
    char buffer[ buffsize ];
    
    std::pair <char, long long int> frequencies [ 256 ];
    
    long long int old_length_bits = 0;
    long long int new_length_bits = 0;
    int num_unique_chars = 0;

    for ( int i = 0; i < 256; i++ )
    {
        frequencies[ i ].first  = i;
        frequencies[ i ].second = 0;
    } 


    while ( in_f )
    {
        in_f.read ( buffer, buffsize );
        for ( int i = 0; i < in_f.gcount(); i++ )
        {
            int index = ( buffer[ i ] >= 0 ? buffer[ i ] : buffer[ i ] + 256 );
            frequencies[ index ].second += 1;
        }
    }

    in_f.close();

    //sort by frequency
    for ( int i = 0; i < 256; i++ )
    {
        int temp_max = frequencies[ i ].second;
        int temp_max_index = i;
        for ( int j = i + 1; j < 256; j++ )
        {
            if ( frequencies[ j ].second > temp_max )
            {
                temp_max = frequencies[ j ].second;
                temp_max_index = j;
            }
        }
        std::pair <char, long long int> temp = frequencies[ i ];
        frequencies[ i ] = frequencies[ temp_max_index];
        frequencies[ temp_max_index ] = temp;
    }

    //non zero frequencies
    for ( int i = 0; i < 255; i++ )
    {
        if ( frequencies[ i ].second != 0 )
        {
            num_unique_chars++;
        }
    }

    if ( num_unique_chars == 0 )
    {
        std::cerr << "In file appears to be empty. Exiting" << std::endl;
        return 3;
    }

    //fill unordered map
    std::unordered_map< char, std::pair<char, long long int> > huffman_map;

    {
        huffman_tree tree( frequencies );
        tree.fill_unordered_map( huffman_map );
    }
    
    std::fstream out_f( out, std::ios::binary | std::ios::out);
    if ( out_f.fail() )
    {
        std::cerr << "Error opening " << out << std::endl;
        return 4;
    }


    out_f     << "FILE:               " << in               << '\n'
              << "NUM_UNIQUE_SYMBOLS: " << num_unique_chars << '\n'
              << "+-------------+-----------+----START-huffman-codes----+-------------------+" << '\n'
              << "|    ASCII    | Frequency |       HUFFMAN Code        | New Length (bits) |" << '\n'
              << "+-------------+-----------+---------------------------+-------------------+" << std::endl;

    for ( int i = 0; i < 255; i++ )
    {
        if  ( frequencies[ i ].second )
        {

            std::string code = to_binary( huffman_map[ frequencies[ i ].first ].second, huffman_map[ frequencies[ i ].first ].first  );

            out_f     << "| " << std::setw(10) << printable_ASCII( frequencies[ i ].first ) << "  | " 
                      << std::setw(9)  << frequencies[ i ].second << " | " 
                      << std::setw(25) << code << " | "
                      << std::setw(17) << code.length() << " |" << std::endl;

            old_length_bits += 8 * frequencies[ i ].second ;
            new_length_bits += code.length() * frequencies[ i ].second ;

        }
    }

    out_f     << "+-------------+-----------+-----END-huffman-codes-----+-------------------+" << '\n'
              << "Old legnth (bytes): " << std::setw( 12 ) << ( old_length_bits >> 3 ) << '\n'
              << "New length (bytes): " << std::setw( 12 ) << ( new_length_bits >> 3 ) << '\n'
              << "DEFALTION RATE:           " << std::setprecision( 4 ) 
              << static_cast<float>( 100 * new_length_bits ) / old_length_bits << '%' << std::endl;

    return 0;

}

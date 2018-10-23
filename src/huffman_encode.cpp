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
#include <iomanip>
#include <fstream>
#include <memory>
#include <utility>
#include <unordered_map>

#include <unistd.h>

#include "huffman_tree.h"
#include "bit_writer.h"


int main ( int argc, char* argv[])
{

    std::string in;
    std::string out;

    if( isatty( STDIN_FILENO ) == 0 )
    {
        std::cerr << "ENCODE: Input must currently be from a file :(" << std::endl;
        return 1;
        //in = "/dev/stdin";
        //out = ( argc == 1 ? "/dev/stdout" : argv[ 1 ] );
    }

    else if ( argc == 2 || argc == 3 )
    {
        in  =   argv[ 1 ];
        out = ( argc == 2 ? "/dev/stdout" : argv[ 2 ] );
    }
    else
    {
        std::cerr << "ENCODE: Synatx: huffman_encode <infile> <outfile>" << std::endl;
        return 1;
    }

    std::fstream in_f( in, std::ios::binary | std::ios::in );

    if ( in_f.fail() )
    {
        std::cerr << "ENCODE: Error opening " << in << std::endl;
        return 2;
    }

    constexpr int in_buffsize = 256*1024;
    char in_buffer[ in_buffsize ];
    
    //stores ASCII_CODE, FREQUENCY
    std::pair <char, long long int> frequencies [ 256 ];
    long long int old_length_bits = 0;
    long long int new_length_bits = 0;

    for ( int i = 0; i < 256; i++ )
    {
        frequencies[ i ].first  = i;
        frequencies[ i ].second = 0;
    } 

    //do a first pass through the file counting frequencies
    while ( in_f )
    {
        in_f.read ( in_buffer, in_buffsize );
        for ( int i = 0; i < in_f.gcount(); i++ )
        {
            int index = ( in_buffer[ i ] >= 0 ? in_buffer[ i ] : in_buffer[ i ] + 256 );
            frequencies[ index ].second += 1;
        }
    }

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
    
    //number the non zero frequencies
    int num_unique_chars = 0;
    
    for ( int i = 0; frequencies[ i ].second; i++ )
    {
        num_unique_chars++;
    }

    if ( num_unique_chars == 0 )
    {
        std::cerr << "ENCODE: In file appears to be empty. Exiting" << std::endl;
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
        std::cerr << "ENCODE: Error opening " << out << std::endl;
        return 4;
    }

    //write the number of symbol codes to the file
    out_f << static_cast<char>( ( num_unique_chars >> 8 ) & 0xFF ) <<  static_cast<char>( ( num_unique_chars >> 0 ) & 0xFF );
    new_length_bits += 2;

    //print symbol codes to the file
    //SYMBOL[ 1 byte ]  LENGTH[ 1 byte ]  CODE[ 8 bytes ]
    for ( int i = 0; i < num_unique_chars; i++ )
    {
       out_f << static_cast<char>( frequencies[ i ].first )
             << static_cast<char>( huffman_map[ ( frequencies[ i ].first ) ].first );

       for ( int j = 0; j < 8; j++ )
            out_f << static_cast<char>( ( huffman_map[ ( frequencies[ i ].first ) ].second >> ( 8 * ( 7 - j ) ) ) & 0xFF );
        new_length_bits += 10 * 8; // 10 bytes per symbol
    }

    out_f.close();
    in_f.clear();
    in_f.seekg( std::ios_base::beg );

    {
        bit_writer b( out );

        //stream data and convert bits using lookup map
        //bit writer queues to write until a multiple of 8 bits is recieved
        while ( in_f )
        {
            in_f.read ( in_buffer, in_buffsize );
            for ( int i = 0; i < in_f.gcount(); i++ )
            {
                b.add_bits( huffman_map[ ( in_buffer[ i ] )].first, huffman_map[ ( in_buffer[ i ] )].second );
                new_length_bits += huffman_map[ ( in_buffer[ i ] )].first;
                old_length_bits += 8;
            }
        }
        new_length_bits += 8; //tag byte
    }

    in_f.close();

    std::cerr << "ENCODE: DEFALTION RATE: " << std::setprecision( 4 ) 
              << static_cast<float>( 100 * new_length_bits ) / old_length_bits << '%' << std::endl;

    return 0;

}
#ifndef ANCILLARY_FUNCTIONS
#define ANCILLARY_FUNCTIONS

#include <functional>
#include <string>

//define hashing pairs so we can use them as keys for unordered maps
//copied from boost library
namespace std 
{
    template<typename T1, typename T2>
    struct hash<std::pair<T1, T2>> 
    {
        std::size_t operator()(std::pair<T1, T2> const &p) const {
        std::size_t seed1(0);
        seed1 ^= std::hash<T1>{}( p.first ) + 0x9e3779b9 
               + (p.second<<6) + (p.second>>1);
    
        std::size_t seed2(0);
        seed2 ^= std::hash<T2>{}(p.second) + 0x9e3779b9 
               + (p.first<<6) + (p.first>>1);


      return (seed1 ^ seed2);
    }
  };
}

template <class T>
std::string to_binary( T num, char bits )
{
    std::string s = "";
    if ( bits == -1 )
    {    
        while ( num != 0 )
        {
            s = (char)( (num&1) + 0x30 ) + s;
            num >>= 1;
        }

        while ( s.length() % 8 != 0 || s.length() == 0 )
        {
            s = "0" + s;
        } 
    }
    else
    {
        while ( bits )
        {
            s = static_cast<char>( (num & 1 ) + 0x30 ) + s;
            num /= 2;
            bits--;
        }
    } 
    return ( s );
}

std::string printable_ASCII ( unsigned char c )
{
    switch ( c )
    {
        case '\a': return ( "\\a " );
        case '\b': return ( "\\b " );
        case '\n': return ( "\\n " );
        case '\r': return ( "\\r " );
        case '\t': return ( "\\t " ); 
        case '\v': return ( "\\v " );
        case ' ' : return ("\" \"");
        default:
            std::string s;
            if ( c >= 0x20 && c <=0x7E )
            {
                s.push_back( c );
                s += "  ";
                return ( s );
            }
            else
            {
                char high  = ( ( c & 0xF0 ) >> 4 );
                char low   = ( ( c & 0x0F ) );
                s+= ( high >= 10 ? high+0x37 : high+0x30);
                s+= ( low  >= 10 ? low +0x37 : low +0x30);
                return "0x" + s;
            }
    }
}

#endif

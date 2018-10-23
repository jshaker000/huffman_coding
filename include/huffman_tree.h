#ifndef HUFFMAN_TREE 
#define HUFFMAN_TREE

#include <memory>
#include <utility>
#include <unordered_map>

//pointers to its children, left and right
//the frequency of occurance of it and its children
//the letters contained under it and all its children
struct huffman_node
{
    std::unique_ptr <struct huffman_node> left  = 0;
    std::unique_ptr <struct huffman_node> right = 0;
    int frequency = 0;
    char data = 0;
};

//expects to take in a sorted array of pairs
// <symbol, frequency>
class huffman_tree
{
    public:
        huffman_tree( std::pair <char, long long int> * );
        void fill_unordered_map ( std::unordered_map< char, std::pair<char, long long int> > & );
    private:
        void recursive_fill ( std::unordered_map< char, std::pair<char, long long int> > &, huffman_node*, char, long long int );
        std::unique_ptr<struct huffman_node> root_node;

};

#endif
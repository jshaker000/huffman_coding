#ifndef HUFFMAN_TREE
#define HUFFMAN_TREE

#include <cstdint>
#include <memory>
#include <utility>
#include <unordered_map>

//expects to take in a sorted array of pairs
// <symbol, frequency>
class huffman_tree
{
    public:
        huffman_tree(std::pair <char, std::uint64_t> frequencies [256]);
        void fill_unordered_map (std::unordered_map<char, std::pair<char, std::uint64_t>> & );
    private:
       //pointers to its children, left and right
       //the frequency of occurance of it and its children
       //the letters contained under it and all its children
        struct huffman_node
        {
            std::unique_ptr <struct huffman_tree::huffman_node> left  = nullptr;
            std::unique_ptr <struct huffman_tree::huffman_node> right = nullptr;
            int frequency = 0;
            char data = 0;
        };
        void recursive_fill (std::unordered_map<char, std::pair<char, std::uint64_t>> &, const huffman_tree::huffman_node *, std::uint8_t, std::uint64_t) const;
        std::unique_ptr<struct huffman_tree::huffman_node> root_node;
};

#endif

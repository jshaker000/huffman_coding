#ifndef HUFFMAN_TREE
#define HUFFMAN_TREE

#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <unordered_map>

// expects to take in a descended sorted array of pairs
// <symbol, frequency> by frequency
// and then be used to parse into an unordered map of
// <symbol, <length,encoding>> for encoding
class huffman_tree
{
    public:
        huffman_tree(const std::array<std::pair <char, std::uint64_t>,256> &);
        void fill_unordered_map (std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> & );
    private:
       //pointers to its children, left and right
       //the frequency of occurance of it and its children
       //the letters contained under it and all its children
        struct huffman_node
        {
            std::unique_ptr <struct huffman_tree::huffman_node> left  = nullptr;
            std::unique_ptr <struct huffman_tree::huffman_node> right = nullptr;
            std::uint64_t frequency = 0;
            char data = 0;
        };

        //prohibit copy constructor
        huffman_tree    (const huffman_tree &);
        void   operator=(huffman_tree);

        void recursive_fill (std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> &, const huffman_tree::huffman_node *, std::uint8_t, std::uint64_t) const;
        std::unique_ptr<struct huffman_tree::huffman_node> root_node;
};

#endif

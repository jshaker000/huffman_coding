#ifndef HUFFFMAN_DECODE_TREE
#define HUFFFMAN_DECODE_TREE

#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

namespace huffman
{
    class huffman_decode_tree
    {
        public:
            // takes a vector of tuples 'symbol, length, position' and maps into a tree
            huffman_decode_tree(const std::vector<std::tuple <char, std::uint8_t, std::uint64_t>> &);
            enum class Direction{RESET, LEFT, RIGHT, NONE};
            // moving working_ptr in direction d and return if the new position is valid (non nullptr),
            // is a leaf, and the data under it.
            std::tuple <bool, bool, char> move_direction(huffman_decode_tree::Direction);
        private:
           //pointers to its children, left and right
           //the frequency of occurance of it and its children
           //the letters contained under it and all its children
            struct huffman_node
            {
                std::unique_ptr <struct huffman_decode_tree::huffman_node> left  = nullptr;
                std::unique_ptr <struct huffman_decode_tree::huffman_node> right = nullptr;
                char data               = 0x00;
            };

            //prohibit copy constructor
            huffman_decode_tree(const huffman_decode_tree &);
            void   operator=(huffman_decode_tree);

            std::unique_ptr<struct huffman_decode_tree::huffman_node> root_node = nullptr;
            struct huffman_decode_tree::huffman_node *working_ptr = nullptr;
    };
}

#endif

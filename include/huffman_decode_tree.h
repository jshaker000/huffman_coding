#ifndef HUFFFMAN_DECODE_TREE
#define HUFFFMAN_DECODE_TREE

#include <cstdint>
#include <memory>
#include <vector>

namespace huffman
{
    struct symbol_len_encode
    {
        char symbol;
        std::uint8_t  length;
        std::uint64_t encoding;
    };
    struct decode_status
    {
        bool is_valid;
        bool is_leaf;
        char symbol;
    };
    class huffman_decode_tree
    {
        public:
            // takes a vector of tuples 'symbol, length, position' and maps into a tree
            huffman_decode_tree(const std::vector<struct huffman::symbol_len_encode> &);
            enum class Direction{RESET, LEFT, RIGHT, NONE};
            // moving working_ptr in direction d and return if the new position is valid (non nullptr),
            // is a leaf, and the data under it.
            // In this way, you can put in one bit at a time and find if you've reached a leaf or not,
            // then RESET and continue
            struct huffman::decode_status move_direction(huffman_decode_tree::Direction);
        private:
            struct huffman_node
            {
                std::unique_ptr <struct huffman_decode_tree::huffman_node> left  = nullptr;
                std::unique_ptr <struct huffman_decode_tree::huffman_node> right = nullptr;
                char data               = 0x00;
            };

            //prohibit copy constructor
            huffman_decode_tree(const huffman_decode_tree &);
            void   operator=(huffman_decode_tree);

            // root
            std::unique_ptr<struct huffman_decode_tree::huffman_node> root_node = nullptr;
            // where we are working (for move_direction)
            struct huffman_decode_tree::huffman_node *working_ptr = nullptr;
    };
}

#endif

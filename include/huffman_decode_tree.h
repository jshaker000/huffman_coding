#ifndef HUFFFMAN_DECODE_TREE
#define HUFFFMAN_DECODE_TREE

#include <cstdint>
#include <memory>
#include <vector>

namespace huffman
{
    class huffman_decode_tree
    {
        public:

            struct symbol_len_encode
            {
                char symbol;
                std::uint8_t  length;
                std::uint64_t encoding;
            };
            huffman_decode_tree(const std::vector<struct huffman::huffman_decode_tree::symbol_len_encode> &);
            // moving working_ptr in direction d and return if the new position is valid (non nullptr),
            // if it is a leaf, and the data under it.
            // In this way, you can put in one bit at a time and find if you've reached a leaf or not,
            // then Direction::RESET and continue
            // you can go in Direction::NONE to reobserve the current state
            enum class Direction{RESET, LEFT, RIGHT, NONE};
            struct decode_status
            {
                bool is_valid;
                bool is_leaf;
                char symbol;
            };
            struct huffman::huffman_decode_tree::decode_status move_direction(huffman_decode_tree::Direction);
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

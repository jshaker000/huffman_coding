#ifndef HUFFFMAN_DECODE_TREE
#define HUFFFMAN_DECODE_TREE

#include <cstdint>
#include <memory>
#include <vector>

namespace huffman
{
    class decode_tree
    {
        public:

            struct symbol_len_encode
            {
                char symbol;
                std::uint8_t  length;
                std::uint64_t encoding;
            };
            // construct tree from <symbol, length of encoding, encoding>
            // Does not check if the tree is valid
            decode_tree(const std::vector<struct huffman::decode_tree::symbol_len_encode> &);
            // moving working_ptr in direction d and return if the new position is valid (non nullptr),
            // if it is a leaf, and the data under it.
            // In this way, you can put in one bit at a time and find if you've reached a leaf or not,
            // and then output that symbol.
            // Then go in Direction::RESET and input the next sequence
            // you can go in Direction::NONE to reobserve the current state
            enum class Direction{RESET, LEFT, RIGHT, NONE};
            struct decode_status
            {
                bool is_valid;
                bool is_leaf;
                char symbol;
            };
            struct huffman::decode_tree::decode_status move_direction(decode_tree::Direction);
        private:
            struct huffman_node
            {
                std::unique_ptr <struct decode_tree::huffman_node> left  = nullptr;
                std::unique_ptr <struct decode_tree::huffman_node> right = nullptr;
                char data               = 0x00;
            };

            //prohibit copy constructor
            decode_tree(const decode_tree &);
            void   operator=(decode_tree);

            // root
            std::unique_ptr<struct decode_tree::huffman_node> root_node = nullptr;
            // where we are working (for move_direction)
            struct decode_tree::huffman_node *working_ptr = nullptr;
    };
}

#endif

#include <iostream>
#include "huffman_decode_tree.h"

// takes in a list of nodes with their symbol, depth, and position and maps into a tree
// right now, no error checking
huffman::decode_tree::decode_tree(const std::vector<struct huffman::decode_tree::symbol_len_encode> &nodes)
{
    root_node.reset(new huffman::decode_tree::huffman_node);
    for (auto const &n : nodes)
    {
        struct huffman::decode_tree::huffman_node *tmp_ptr = root_node.get();
        for (std::uint8_t i = 0; i < n.length; i++)
        {
            std::uint8_t bit = (n.encoding >> (n.length-1-i)) & 0x01;
            if (bit == 0x00)
            {
                if (tmp_ptr->left == nullptr)
                {
                    tmp_ptr->left.reset(new huffman::decode_tree::huffman_node);
                }
                tmp_ptr = tmp_ptr->left.get();
            }
            else
            {
                if(tmp_ptr->right == nullptr)
                {
                    tmp_ptr->right.reset(new huffman::decode_tree::huffman_node);
                }
                tmp_ptr = tmp_ptr->right.get();
            }
            if (i == n.length - 1)
            {
                tmp_ptr->data = n.symbol;
            }
        }
    }
    working_ptr = root_node.get();
}

// moving working_ptr in direction d and return if the new position is valid (non nullptr),
// is a leaf, and the data under it.
// Used so you to can traverse the tree, find data, reset, etc to decode huffman encoded file
// you can go in Direction::NONE to reobserve the current state
struct huffman::decode_tree::decode_status huffman::decode_tree::move_direction(huffman::decode_tree::Direction d)
{
    if (d == huffman::decode_tree::Direction::RESET)
    {
        working_ptr = root_node.get();
    }
    else if (d == huffman::decode_tree::Direction::LEFT)
    {
        working_ptr = (working_ptr->left).get();
    }
    else if (d == huffman::decode_tree::Direction::RIGHT)
    {
        working_ptr = (working_ptr->right).get();
    }
    else // d == huffman::decode_tree::Direction::NONE
    {
        ;
    }

    bool is_valid = working_ptr != nullptr;
    bool is_leaf  = is_valid && working_ptr->left == nullptr && working_ptr->right == nullptr;
    char symbol   = is_leaf ? working_ptr->data : 0x00;

    struct huffman::decode_tree::decode_status ds{ is_valid, is_leaf, symbol };

    return ds;
}

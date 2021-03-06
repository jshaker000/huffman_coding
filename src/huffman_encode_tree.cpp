#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "huffman_encode_tree.h"

// takes in a descended sorted by frequency array of <symbol, frequency>
// Generates a huffman tree
huffman::encode_tree::encode_tree(const std::array<struct huffman::encode_tree::symb_freq,256> & frequency)
{
    // find the bottommost nonzero frequency symbol
    int num_unique = 0;
    for (const auto &f: frequency)
    {
        if (f.frequency != 0)
        {
            num_unique++;
        }
    }

    //special case of input file only having 0/1 distinct character
    if (num_unique == 0) // error
    {
        std::cerr << "Huffman Encode Tree: input frequency map seems to be empty!" << std::endl;
        std::exit(-1);
    }
    if (num_unique == 1) // right now, each node needs 0 or 2 children. So invert the symbol to make the other one
    {
        root_node.reset(new huffman::encode_tree::huffman_node);
        root_node->right.reset(new huffman::encode_tree::huffman_node);
        root_node->left.reset(new huffman::encode_tree::huffman_node);

        root_node->right->data = frequency[0].symbol;
        root_node->left->data  = frequency[0].symbol ^ static_cast<char>(0xFF);

        root_node->frequency        = frequency[0].frequency;
        root_node->right->frequency = frequency[0].frequency;
        root_node->left->frequency  = 0;
        return;
    }

    // build a list of nodes for each symbol, in decending order
    std::vector<std::unique_ptr<struct huffman::encode_tree::huffman_node>> node_list;
    node_list.reserve(num_unique);

    std::for_each(frequency.begin(), frequency.begin() + num_unique,
    [&node_list] (const auto &f)
    {
        std::unique_ptr<struct huffman::encode_tree::huffman_node> tmp(new struct huffman::encode_tree::huffman_node);
        tmp->data      = f.symbol;
        tmp->frequency = f.frequency;
        node_list.push_back(std::move(tmp));
    });

    // loop through the list of nodes and take the bottom two and put them into a new node until
    // everything is under one big tree
    for (int i = 0; i < num_unique - 1; i++)
    {
        std::unique_ptr<struct huffman::encode_tree::huffman_node> inner_node(new struct huffman::encode_tree::huffman_node);

        inner_node->right = std::move(node_list[num_unique-i-2]);
        inner_node->left  = std::move(node_list[num_unique-i-1]);
        inner_node->frequency = inner_node->right->frequency + inner_node->left->frequency;

        // find where to insert and push thing back as needed
        for (int j = 0; j < num_unique-i-1; j++)
        {
            if (j == num_unique-i-2 || inner_node->frequency > node_list[j]->frequency)
            {
                for (int k = num_unique-i-2; k > j; k--)
                {
                    node_list[k] = std::move(node_list[k-1]);
                }
                node_list[j] = std::move(inner_node);
                break;
            }
        }
    }
    root_node = std::move(node_list[0]);
}

// finds all of the leaf nodes and inserts them, along with their symbol into an unordered map
// symbol,  pair <len, code in binary>
// for easy encoding
void huffman::encode_tree::fill_unordered_map(std::unordered_map<char, struct huffman::encode_tree::len_encode> &map) const
{
    recursive_fill(map, root_node.get(), 0, 0);
}

void huffman::encode_tree::recursive_fill(std::unordered_map<char, struct huffman::encode_tree::len_encode> & map,
                                          const huffman::encode_tree::huffman_node* const root,
                                          std::uint8_t  len,
                                          std::uint64_t huffman_code)
const
{
    if (len > 64)
    {
        std::cerr << "Huffman Encode Tree: Tree depths >= 64 are not currently supported" << std::endl;
        std::exit(-1);
    }
    else if (root->left == nullptr && root->right == nullptr)
    {
        struct huffman::encode_tree::len_encode lc{len, huffman_code};
        map.emplace(root->data, lc);
        return;
    }
    else
    {
        recursive_fill(map, root->left.get(),  len+1, (huffman_code<<1) + 0x00);
        recursive_fill(map, root->right.get(), len+1, (huffman_code<<1) + 0x01);
    }
}

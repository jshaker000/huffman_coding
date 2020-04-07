#include <cstdlib>
#include <iostream>
#include <vector>

#include "huffman_tree.h"

// assumes frequency is sorted in decending order
huffman_tree::huffman_tree(const std::array<std::pair <char, std::uint64_t>,256> & frequency)
{
    // find the bottommost nonzero frequency symbol
    int position = -1;
    while (position != 255 && frequency[position+1].second != 0)
        position++;

    const int num_unique = position + 1;

    //special case of input file only having 0/1 distinct character
    if (num_unique == 0)
    {
        std::cerr << "Huffman Tree: input frequency map seems to be empty!" << std::endl;
        std::exit(-1);
    }
    if (num_unique == 1)
    {
        root_node.reset(new huffman_tree::huffman_node);
        root_node->right.reset(new huffman_tree::huffman_node);
        root_node->left.reset(new huffman_tree::huffman_node);

        root_node->right->data = frequency[0].first;
        root_node->left->data  = frequency[0].first ^ static_cast<char>(0xFF);
        return;
    }

    //build a list of nodes for each symbol, in decending order
    std::vector<std::unique_ptr<struct huffman_tree::huffman_node>> node_list;
    node_list.reserve(num_unique);

    for (int i = 0; i < num_unique; i++)
    {
        std::unique_ptr<struct huffman_tree::huffman_node> tmp(new struct huffman_tree::huffman_node);
        tmp->data      = frequency[i].first;
        tmp->frequency = frequency[i].second;
        node_list.push_back(std::move(tmp));
    }

    // loop through the list of nodes and take the bottom two and put them into a new node until
    // everything is under one big tree
    for (int i = 0; i < num_unique - 1; i++)
    {
        std::unique_ptr<struct huffman_tree::huffman_node> inner_node(new huffman_tree::huffman_node);

        inner_node->left  = std::move(node_list[num_unique-i-1]);
        inner_node->right = std::move(node_list[num_unique-i-2]);
        inner_node->frequency = inner_node->right->frequency + inner_node->left->frequency;

        // find where to insert
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

void huffman_tree::fill_unordered_map(std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> & map)
{
    recursive_fill(map, root_node.get(), 0, 0);
}

// finds all of the leaf nodes and inserts them, along with their symbol into an unordered map
// symbol,  pair <length, code in binary>
void huffman_tree::recursive_fill(std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> & map,
                                  const huffman_tree::huffman_node* const root,
                                  std::uint8_t  length,
                                  std::uint64_t huffman_code)
const
{
    if (length > 64)
    {
        std::cerr << "Huffman Tree: Tree depths >= 64 are not currently supported" << std::endl;
        std::exit(-1);
    }
    if (root->left == nullptr && root->right == nullptr)
    {
        map.emplace(root->data, std::pair<std::uint8_t,std::uint64_t> (length, huffman_code));
        return;
    }
    else
    {
        recursive_fill(map, root->left.get(),  length+1, (huffman_code*2) + 0x00);
        recursive_fill(map, root->right.get(), length+1, (huffman_code*2) + 0x01);
    }
}

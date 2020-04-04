#include <vector>

#include "huffman_tree.h"

huffman_tree::huffman_tree(const std::array<std::pair <char, std::uint64_t>,256> & frequency)
{
    //finds the bottommost nonzero frequency symbol
    int position = frequency.size() - 1;
    while (position && frequency[position].second == 0)
        position--;

    //special case of input file only having one character
    if ( position <= 0 )
    {
        root_node.reset ( new huffman_tree::huffman_node );
        root_node->right.reset ( new huffman_tree::huffman_node );
        root_node->left.reset  ( new huffman_tree::huffman_node );

        root_node->right->data = frequency[ 0 ].first;
        root_node->left->data = ( frequency[ 0 ].first ^ 0xFF );
        return;
    }

    //build a list of nodes
    std::vector< std::unique_ptr<struct huffman_tree::huffman_node> > node_list;

    for ( int i = 0; i < position + 1; i++ )
    {
        std::unique_ptr<struct huffman_tree::huffman_node> temp( new struct huffman_tree::huffman_node );
        node_list.push_back( std::move( temp ) );
        node_list[i]->data      = frequency[i].first;
        node_list[i]->frequency = frequency[i].second;
        //std::cerr << node_list[i]->data << '\t' << node_list[i]->frequency << std::endl;
    }

    //loop through the list of nodes and take the bottom two and put them into a new node until 
    //everything is under one big tree
    while ( node_list.size() != 1 )
    {
        std::unique_ptr<struct huffman_tree::huffman_node> inner_node( new huffman_tree::huffman_node );

        inner_node->left  = std::move( node_list[ position ] );
        inner_node->right = std::move( node_list[ position - 1 ] );
        inner_node->frequency = inner_node->right->frequency + inner_node->left->frequency;

        node_list.pop_back();

        //  std::cerr << "current position: " << position << '\n'
        //            << '\t' << inner_node->left->data << ":" << inner_node->left->frequency
        //            << "+" << inner_node->right->data << ":" << inner_node->right->frequency << " = " << inner_node->frequency << std::endl;
        for ( int i = 0; i < position; i++ )
        {
            if ( node_list[i] == NULL || inner_node->frequency > node_list[i]->frequency )
            {
                for ( int j = position; j > i && position != 1; j-- )
                {
                    node_list[ j ] = std::move( node_list[ j - 1 ] );
                }
                //std::cerr << "\tWriting to index " << i << std::endl;
                node_list[i] = std::move( inner_node );
                break;
            }
        }
        position--;
    }
    root_node = std::move(node_list[0]);
}

void huffman_tree::fill_unordered_map(std::unordered_map<char, std::pair<char, std::uint64_t>> & map)
{
    recursive_fill(map, root_node.get(), 0, 0);
}

//finds all of the leaf nodes and inserts them, along with their code into an unordered map
// [symbol],  pair < length, code in binary >
void huffman_tree::recursive_fill( std::unordered_map< char, std::pair<char, std::uint64_t> > & map,
                                   const huffman_tree::huffman_node* const root, 
                                   std::uint8_t length,
                                   std::uint64_t huffman_code)
const
{
    if (root->left == nullptr && root->right == nullptr)
    {
        std::pair<char, std::uint64_t> p (length, huffman_code);
        map.emplace(root->data, p);
        return;
    }

    else
    {
        recursive_fill(map, root->left.get(),  length+1, (huffman_code<<1) + 0);
        recursive_fill(map, root->right.get(), length+1, (huffman_code<<1) + 1);
    }

}

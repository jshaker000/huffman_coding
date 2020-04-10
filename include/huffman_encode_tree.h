#ifndef HUFFMAN_ENCODE_TREE
#define HUFFMAN_ENCODE_TREE

#include <array>
#include <cstdint>
#include <memory>
#include <utility>
#include <unordered_map>

namespace huffman
{
    class huffman_encode_tree
    {
        public:
            // expects array of <symbol,frequency> pairs sorted in descending order by frequency
            // used to fill a map of chars -> (length, code)
            // to easily read in bytes and encode them
            huffman_encode_tree(const std::array<std::pair <char, std::uint64_t>,256> &);
            void fill_unordered_map (std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> & );
        private:
            struct huffman_node
            {
                std::unique_ptr <struct huffman_encode_tree::huffman_node> left  = nullptr;
                std::unique_ptr <struct huffman_encode_tree::huffman_node> right = nullptr;
                std::uint64_t frequency = 0;
                char data               = 0x00;
            };

            //prohibit copy constructor
            huffman_encode_tree    (const huffman_encode_tree &);
            void   operator=(huffman_encode_tree);

            void recursive_fill (std::unordered_map<char, std::pair<std::uint8_t, std::uint64_t>> &, const huffman_encode_tree::huffman_node *, std::uint8_t, std::uint64_t) const;
            // root node
            std::unique_ptr<struct huffman_encode_tree::huffman_node> root_node;
    };
}

#endif
#ifndef HUFFMAN_ENCODE_TREE
#define HUFFMAN_ENCODE_TREE

#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace huffman
{

    class encode_tree
    {
        public:
            // expects array of <symbol,frequency> pairs sorted in descending order by frequency
            // used to fill a map of chars -> (length, code)
            // to easily read in bytes and encode them
            struct symb_freq
            {
                char symbol;
                std::uint64_t frequency;
            };
            encode_tree(const std::array<struct huffman::encode_tree::symb_freq,256> &);
            struct len_encode
            {
                std::uint8_t  length;
                std::uint64_t encoding;
            };
            void fill_unordered_map (std::unordered_map<char, struct huffman::encode_tree::len_encode> &) const;
        private:
            struct huffman_node
            {
                std::unique_ptr <struct encode_tree::huffman_node> left  = nullptr;
                std::unique_ptr <struct encode_tree::huffman_node> right = nullptr;
                std::uint64_t frequency = 0;
                char data               = 0x00;
            };

            //prohibit copy constructor
            encode_tree(const encode_tree &);
            void   operator=   (encode_tree);

            void recursive_fill (std::unordered_map<char, struct huffman::encode_tree::len_encode> &,
                                const encode_tree::huffman_node *,
                                std::uint8_t,
                                std::uint64_t) const;
            // root node
            std::unique_ptr<struct encode_tree::huffman_node> root_node;
    };
}

#endif

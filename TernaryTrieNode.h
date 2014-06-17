#include <iterator>

template <class Key, class Value>
class TernaryTrie;

template <class Key, class Value>
class TernaryTrieNode {
    friend class TernaryTrie<Key,Value>;
    TernaryTrieNode *left_, *right_, *equal_, *parent_;
    typename TernaryTrie<Key,Value>::value_type data; // if data.first.empty(), no data is present
    unsigned char c;
public:
    TernaryTrieNode ( TernaryTrieNode *parent, unsigned char c_ ) : left_(nullptr), right_(nullptr), equal_(nullptr), parent_(parent), c(c_) {}
    ~TernaryTrieNode();
};

template <class Key, class Value>
TernaryTrieNode<Key, Value>::~TernaryTrieNode() {
    delete left_;
    delete equal_;
    delete right_;
}


template <class Key, class Value>
class TernaryTrieNodeIterator : public std::iterator<std::bidirectional_iterator_tag, TernaryTrieNode<Key,Value> *>
{
    //TODO
};

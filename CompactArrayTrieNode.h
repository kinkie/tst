#ifndef SQUID_COMPACTARRAYTRIENODE_H_
#define SQUID_COMPACTARRAYTRIENODE_H_

#include <iterator>
#include <vector>

template <class Key, class Value> class Trie;

template <class Key, class Value>
class CompactArrayTrieNode
{
public:
    typedef typename Trie<Key,Value>::value_type value_type;
    CompactArrayTrieNode();
    ~CompactArrayTrieNode();
    // should probably use a proper iterator
    value_type *find (Key const &, size_t pos, bool const prefix) const;
    bool add (Key const &, size_t pos, Value &);

private:
    typedef std::vector<CompactArrayTrieNode *> children_t;

    children_t children;
    value_type data;
    int offset;
    bool haveData;

    /* not implemented */
    CompactArrayTrieNode(const CompactArrayTrieNode&);
    CompactArrayTrieNode& operator= (CompactArrayTrieNode const &);

};

template <class Key, class Value>
CompactArrayTrieNode<Key,Value>::CompactArrayTrieNode() :
    offset(0), haveData(false)
{}

template <class Key, class Value>
typename CompactArrayTrieNode<Key,Value>::value_type *
CompactArrayTrieNode<Key,Value>::find (Key const & key, size_t pos, bool const prefix) const
{
    if (pos < key.size()) {
        // todo: charTransform?
        int char = key[pos];
        int realPos = -1;

        if (pos >= offset && pos - offset < children.size() )
            realPos = pos - offset;
        if (realPos >= 0 && children[realPos]) {
            value_type *result = children[realPos]->find(key, pos+1, prefix);
            if (result)
                return result;
        }
        if (prefix && haveData)
            return &data;
        return NULL;
    } else {
        if (haveData)
            return &data;
        return NULL;
    }
}

#endif /* SQUID_COMPACTARRAYTRIENODE_H_ */

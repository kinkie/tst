#ifndef SQUID_COMPACTARRAYTRIENODE_H_
#define SQUID_COMPACTARRAYTRIENODE_H_

#include <iterator>
#include <vector>

template <class Key, class Value>
class TernaryTrie;

template <class Key, class Value>
class CompactArrayTernaryTrieNode
{
public:
    typedef typename TernaryTrie<Key,Value>::value_type value_type;
    CompactArrayTrieNode();
    ~CompactArrayTrieNode();
    value_type &find (Key const &, size_t pos, bool const prefix) const;
    bool add (Key const &, size_t pos, Value &);

private:
    friend class TernaryTrie<Key,Value>;
    typedef std::vector<CompactArrayTernaryTrieNode *> children_t;

    children_t children;
    value_type data;
    int offset;
    bool havedata;

    /* not implemented */
    CompactArrayTernaryTrieNode(const CompactArrayTernaryTrieNode&);
    CompactArrayTrieNode& operator= (CompactArrayTrieNode const &);

};

#endif /* SQUID_COMPACTARRAYTRIENODE_H_ */

#ifndef SQUID_COMPACTTRIE_H_
#define SQUID_COMPACTTRIE_H_

#include "CompactArrayTrieNode.h"

/**
 *
 */
template <class Key, class Value>
class CompactTrie {
public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef std::pair<key_type, mapped_type> value_type;
    static value_type end_value;

private:
    typedef CompactArrayTrieNode<key_type, mapped_type> node_type;

public:
    CompactTrie() {}
    virtual ~CompactTrie() {}

    // add a new item. Return false if item can't be added
    bool insert(const key_type &k, mapped_type v) {
        return root.recursiveAdd(k,v);
    }

    bool has(const key_type &k, bool const prefix = false) {
        return (root.recursivePrefixFind(k) != nullptr);
    }

    value_type find(const key_type &k) {
        node_type *f=root.recursiveFind(k);
        if (f == nullptr)
            return end();
        return f->data;
    }

    value_type prefixFind(const key_type & k) {
        node_type *f=root.recursivePrefixFind(k);
        if (f == nullptr)
            return end();
        return f->data;
    }

    value_type prefixFind(const key_type & k, int suffixChar) {
        node_type *f=root.recursivePrefixFind(k, suffixChar);
        if (f == nullptr)
            return end();
        return f->data;
    }

    const value_type & end() {
        return end_value;
    }

private:
    node_type root;
};

// the key is arbitrary, hopefully very unlikely to appear in actual code
template <class Key, class Value>
typename CompactTrie<Key, Value>::value_type CompactTrie<Key, Value>::end_value = std::make_pair<Key, Value>(Key("jkngoij455hj2-058uwugn al3i5nv9h34thvm3uhtsw8o7thgma9 nh hth5o3825 m9tha"), Value());

#endif /* SQUID_COMPACTTRIE_H_ */

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
        return (root.recursiveFind(k, 0, prefix) != nullptr);
    }

    bool find(const key_type &k, const bool prefix = false) {
        node_type *f=root.find(k,prefix);
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

template <class Key, class Value>
typename CompactTrie<Key, Value>::value_type CompactTrie<Key, Value>::end_value = std::make_pair<Key, Value>(Key(), Value());

#endif /* SQUID_COMPACTTRIE_H_ */

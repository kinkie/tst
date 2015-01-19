#ifndef SQUID_COMPACTTRIE_H_
#define SQUID_COMPACTTRIE_H_

#include "CompactArrayTrieNode.h"

#include <cassert>

template <class Key, class Value>
class CompactTrieIterator;

/**
 *
 */
template <class Key, class Value>
class CompactTrie {
public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef std::pair<key_type, mapped_type> value_type;
    typedef CompactTrieIterator<key_type, mapped_type> iterator;

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

    iterator find(const key_type &k) {
        node_type *f=root.recursiveFind(k);
        if (f == nullptr)
            return end();
        return iterator(f);
    }

    iterator prefixFind(const key_type & k) {
        node_type *f=root.recursivePrefixFind(k);
        if (f == nullptr)
            return end();
        return iterator(f);
    }

    iterator prefixFind(const key_type & k, int suffixChar) {
        node_type *f=root.recursivePrefixFind(k, suffixChar);
        if (f == nullptr)
            return end();
        return iterator(f);
    }

    iterator end()
    {
        static CompactArrayTrieNode<Key,Value> endnode;
        static typename CompactTrie<Key, Value>::iterator enditer(&endnode);
        return enditer;
    }

private:
    node_type root;
};

template <class Key, class Value>
class CompactTrieIterator
{
public:
    typedef typename CompactTrie<Key,Value>::value_type value_type;
    CompactTrieIterator() : node(nullptr) {} // will bomb on dereferencing
    CompactTrieIterator(const CompactTrieIterator& c) : node(c.node) {}
    CompactTrieIterator& operator=(const CompactTrieIterator &c) { node = c.node; return *this;}
    bool operator==(const CompactTrieIterator& c) const { return node == c.node; }
    bool operator!=(const CompactTrieIterator& c) const { return node != c.node; }
    value_type & operator*() const { assert(node && node->haveData); return node->data; }
    value_type * operator->() const { assert(node && node->haveData); return &(node->data); }
private:
    friend class CompactTrie<Key, Value>;
    explicit CompactTrieIterator(CompactArrayTrieNode <Key,Value> *n) : node(n) {}
    CompactTrieIterator& operator++();
    CompactArrayTrieNode<Key,Value> *node;
};

#endif /* SQUID_COMPACTTRIE_H_ */

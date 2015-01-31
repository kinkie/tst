#ifndef SQUID_COMPACTTRIE_H_
#define SQUID_COMPACTTRIE_H_

#include "CompactArrayTrieNode.h"

#include <cassert>
#include <algorithm>

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
        contentsCache.clear();
        return root.insert(k,v);
    }

    bool has(const key_type &k, bool const prefix = false) {
        return (root.findPrefix(k) != nullptr);
    }

    iterator find(const key_type &k) {
        node_type *f=root.find(k);
        if (f == nullptr)
            return end();
        return iterator(f);
    }

    iterator prefixFind(const key_type & k) {
        node_type *f=root.findPrefix(k);
        if (f == nullptr)
            return end();
        return iterator(f);
    }

    iterator prefixFind(const key_type & k, int suffixChar) {
        node_type *f=root.findPrefix(k, suffixChar);
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

    bool empty() const {
        return root.empty();
    }

    /// return a vector of iterators to all valid contents in the trie
    const std::vector<iterator> & contents();

    //TODO: add delete

private:
    node_type root;
    std::vector<iterator> contentsCache; // valid if !empty() || root.empty()
};

template <class Key, class Value>
const std::vector<typename CompactTrie<Key,Value>::iterator> &
CompactTrie<Key,Value>::contents()
{
    // we have valid cached contents
    if (!contentsCache.empty() || root.empty())
        return contentsCache;

    // possible optimization: keep a count of inserted/removed entries
    // and preallocate the vector size correctly
    std::vector<node_type *> v;
    root.recursivePreorderWalk(v);
    contentsCache.clear();
    contentsCache.reserve(v.size());
    for (auto i = v.begin(); i != v.end(); ++i)
        contentsCache.push_back(iterator(*i));
    return contentsCache;
}

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

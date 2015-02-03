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

    /// add a new item. Return false if item can't be added
    bool insert(const key_type &k, mapped_type v) {
        contentsCache.clear();
        return root.insert(k,v);
    }

    /// return true whether the key(prefix is optional second parameter is true)
    /// is present in the Trie.
    bool has(const key_type &k, bool const prefix = false) {
        return has(k.begin(), k.end(), prefix);
    }
    /// iterator-based variant of has()
    template <class InputIterator>
    bool has(InputIterator begin, const InputIterator &end, bool const prefix = false) {
        if (prefix)
            return (root.findPrefix(begin, end) != nullptr);
        return (root.find(begin, end) != nullptr);
    }

    /// return an iterator to value_type (aka std::pair<key,value>)
    /// corresponding to the supplied key, or end() if not found.
    iterator find(const key_type &k) {
        return find(k.begin(), k.end());
    }
    /// iterator-based variant of find()
    template <class InputIterator>
    iterator find(InputIterator begin, const InputIterator& end) {
        node_type *f=root.find(begin,end);
        if (f == nullptr)
            return this->end();
        return iterator(f);
    }

    /// return an iterator to value_type (aka std::pair<key,value>)
    /// corresponding to the SHORTEST PREFIX of the supplied key in the Trie,
    /// or end() if not found.
    iterator prefixFind(const key_type & k) {
        return prefixFind(k.begin(),k.end());
    }
    /// iterator-based variant of prefixFind()
    template <class InputIterator>
    iterator prefixFind(InputIterator begin, const InputIterator& end) {
        node_type *f=root.findPrefix(begin,end);
        if (f == nullptr)
            return this->end();
        return iterator(f);
    }

    /// return an iterator to value_type (aka std::pair<key,value>)
    /// corresponding to the SHORTEST PREFIX of the supplied key in the Trie,
    /// where the prefix ends with the supplied suffixChar, OR corresponding
    /// to the full key if present, OR to the full key plus suffixChar
    /// e.g. prefixFind("foo%bar%gazonk",'%') will return an iterator to
    /// any of (if present, in order of preference):
    /// foo%, foo%bar%, foo%bar%gazonk, foo%bar%gazonk%
    iterator prefixFind(const key_type & k, int suffixChar) {
        return prefixFind(k.begin(), k.end(), suffixChar);
    }
    /// iterator-based variant of prefixFind(suffixChar)
    template <class InputIterator>
    iterator prefixFind(InputIterator begin, const InputIterator& end, int suffixChar) {
        node_type *f=root.findPrefix(begin, end, suffixChar);
        if (f == nullptr)
            return this->end();
        return iterator(f);
    }

    /// end-iterator, conforming to STL end(), except that
    /// TrieA.end() == TrieB.end() for any TrieA, TrieB
    iterator end()
    {
        static CompactArrayTrieNode<Key,Value> endnode;
        static typename CompactTrie<Key, Value>::iterator enditer(&endnode);
        return enditer;
    }

    /// return true if the Trie is empty
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

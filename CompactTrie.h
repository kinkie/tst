#ifndef SQUID_COMPACTTRIE_H_
#define SQUID_COMPACTTRIE_H_

#include "CompactArrayTrieNode.h"

#include <cassert>
#include <algorithm>

template <class Key, class Value>
class CompactTrieIterator;

/** Associative ordered container oriented to efficient prefix lookups
 *
 * The implementation tries to mimic at least partly the use patterns of std::map.
 * The requirement on the key is that it must be iterable, support begin()
 * and end(), and its iterators' dereference must convert to an int.
 * std::string and SBuf are both valid key types.
 * There is no constraint on the value type, except that it must have
 * by-value semantics (it must clean up after itself in its destructor).
 *
 * \sa http://en.wikipedia.org/wiki/Trie
 * \sa http://www.cplusplus.com/reference/map/map/
 */
template <class Key, class Value>
class CompactTrie {
public:
    typedef Key key_type;
    typedef Value mapped_type;
    // this is the actually-stored data type
    typedef std::pair<key_type, mapped_type> value_type;
    // not really a full iterator; just enough to mimic the most common patterns
    typedef CompactTrieIterator<key_type, mapped_type> iterator;

private:
    // convenience type
    typedef CompactArrayTrieNode<key_type, mapped_type> node_type;

public:
    CompactTrie() {}
    virtual ~CompactTrie() {}

    /** add a new item to the Trie
     *
     * \return false if item can't be added
     */
    bool insert(const key_type &k, mapped_type v) {
        contentsCache.clear();
        return root.insert(k,v);
    }

    /** Check for key or prefix presence
     *
     * \param k the key to be looked up
     * \param prefix if true request a prefix lookup
     * \return true if the key is present in the container; if prefix is true,
     *   then return true if any of the keys stored in the container is
     *   a prefix of k
     */
    bool has(const key_type &k, bool const prefix = false) {
        return has(k.begin(), k.end(), prefix);
    }
    /// check for key/prefix presence, passing the key by begin and end iterators
    template <class InputIterator>
    bool has(InputIterator begin, const InputIterator &end, bool const prefix = false) {
        if (prefix)
            return (root.findPrefix(begin, end) != nullptr);
        return (root.find(begin, end) != nullptr);
    }

    /** key lookup
     *
     * \return iterator to value_type (std::pair<key_type, mapped_type>) if
     *  key is present, or end() if key is not present
     */
    iterator find(const key_type &k) {
        return find(k.begin(), k.end());
    }
    /// key lookup, passing the key by begin and end iterators
    template <class InputIterator>
    iterator find(InputIterator begin, const InputIterator& end) {
        node_type *f=root.find(begin,end);
        if (f == nullptr)
            return this->end();
        return iterator(f);
    }

    /** prefix lookup
     *
     * \return iterator to value_type (std::pair<key_type, mapped_type>)
     *  corresponding to the shortest prefix of the passed argument stored
     *  in the Trie, or end() if no prefix of the argument is found
     */
    iterator prefixFind(const key_type & prefix) {
        return prefixFind(prefix.begin(),prefix.end());
    }
    /// prefix lookup, passing the key by begin and end iterators
    template <class InputIterator>
    iterator prefixFind(InputIterator begin, const InputIterator& end) {
        node_type *f=root.findPrefix(begin,end);
        if (f == nullptr)
            return this->end();
        return iterator(f);
    }

    /** constrained prefix find
     *
     * Search for the stored entry corresponding to the shortest prefix
     * of the supplied key where the prefix ends with the supplied suffixChar
     * or corresponding to the full key or to the full key plus suffixChar.
     * For example, prefixFind("foo%bar%gazonk",'%') will return an iterator to
     * the value_type of (if present, in order of preference):
     * "foo%", "foo%bar%", "foo%bar%gazonk", "foo%bar%gazonk%", end()
     *
     * \return iterator to value_type (std::pair<key_type, mapped_type>)
     *  if any constrained prefix is found, end() if no prefix is found
     *
     */
    iterator prefixFind(const key_type & key, int suffixChar) {
        return prefixFind(key.begin(), key.end(), suffixChar);
    }
    /// constrained prefix lookup, passing the key by begin and end iterators
    template <class InputIterator>
    iterator prefixFind(InputIterator begin, const InputIterator& end, int suffixChar) {
        node_type *f=root.findPrefix(begin, end, suffixChar);
        if (f == nullptr)
            return this->end();
        return iterator(f);
    }

    /** end-iterator
     *
     * This iterator can support the common STL patterns, but it is only
     * a facade. In particular TrieA.end() == TrieB.end() for any TrieA, TrieB
     */
    iterator end()
    {
        static CompactArrayTrieNode<Key,Value> endnode;
        static typename CompactTrie<Key, Value>::iterator enditer(&endnode);
        return enditer;
    }

    /// empty-trie test
    bool empty() const {
        return root.empty();
    }

    /** contents extractor
     *
     * \return std::vector of iterators to value_type
     * (std::pair<key_type,mapped_type>), sorted by key in ascending order
     */
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

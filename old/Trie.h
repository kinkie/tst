#ifndef SQUID_TERNARYTRIE_H_
#define SQUID_TERNARYTRIE_H_

#include "TernaryTrieNode.h"
#include "CompactArrayTrieNode.h"

#include <stdexcept>
#include <utility>

template <class Key, class Value>
class Trie
{
public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef std::pair<Key, Value> value_type;
    typedef unsigned int size_type;

    class TrieIterator {
    public:
    };

    typedef TrieIterator iterator;
    typedef const TrieIterator const_iterator;
    // typedef ... reverse_iterator
    // typedef ... const_reverse_iterator

    Trie();
    ~Trie();

    Trie& operator  = (const Trie &);

    iterator begin() { return TrieIterator(); } // stub
    const_iterator cbegin() const { return TrieIterator(); } // stub
    // iterator end();
    // const_iterator cend() const;
    // reverse_iterator rbegin();
    // const_reverse_iterator crbegin() const;
    // reverse_iterator rend();
    // const_reverse_iterator crend() const;

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type max_size() const; // placeholder

    mapped_type& operator[] (const key_type& k);
    // returns ref to element, throws std::out_of_range if element is not present
    mapped_type& at (const key_type& k) throw (std::out_of_range);
    const mapped_type& at (const key_type& k) const throw (std::out_of_range);

    std::pair<iterator,bool> insert (const value_type& val);
    template <class InputIterator>
    void insert (InputIterator first, InputIterator last);

    //void erase (iterator position);
    size_type erase (const key_type& k);
    //void erase (iterator first, iterator last);

    //void swap (map& x);
    void clear();

    //void makeReadOnly();
    //void optimize();

    //iterator find (const key_type& k);
    //const_iterator find (const key_type& k) const;
    //iterator find_longest_prefix (const key_type& k);
    //const_iterator find_longest_prefix (const key_type& k) const;
    //iterator find_with_prefix_filter(const key_type &prefix);
    //const_iterator find_with_prefix_filter(const key_type &prefix) const;

    size_type count (const key_type& k) const;

private:
    typedef CompactArrayTrieNode<Key,Value> node_type;

    // return a node_type* or nullptr if key is not in the tree
    node_type *getNode(const key_type &key) const;
    node_type *putNode(node_type *n, const value_type& value, int depth = 0);
    node_type *clearSubTree(node_type *n);

    node_type *root;
    size_type size_;
    uint32_t version;
    bool readonly;
};

template <class Key, class Value>
Trie<Key, Value>::Trie() : root(nullptr), size_(0), version(0), readonly(false) { }

template <class Key, class Value>
Trie<Key, Value>::~Trie() {
    delete root;
}

template <class Key, class Value>
const typename Trie<Key, Value>::mapped_type& Trie<Key, Value>::at (const key_type& k)
const throw (std::out_of_range)
{
    if (!root)
        throw std::out_of_range("Element not found");
    node_type * n=getNode(root,k);
    if (n && n->data.first.size())
        return n->data.second;
    throw std::out_of_range("Element not found");
}

template <class Key, class Value>
typename Trie<Key, Value>::mapped_type& Trie<Key, Value>::at (const key_type& k)
throw (std::out_of_range)
{
    if (!root)
        throw std::out_of_range("Element not found");
    //TODO
}

template <class Key, class Value>
typename Trie<Key, Value>::node_type *
Trie<Key, Value>::getNode (const key_type &key) const
{
    if (root == nullptr)
        return nullptr;
    return root->find(key);
}

template <class Key, class Value>
typename std::pair<typename Trie<Key, Value>::iterator, bool>
Trie<Key, Value>::insert(const value_type &val)
{
    //todo
}

template <class Key, class Value>
template <class InputIterator>
void
Trie<Key, Value>::insert(InputIterator first, InputIterator last)
{
    for (InputIterator i = first; i != last; ++i)
        insert(*i);
}

template <class Key, class Value>
typename Trie<Key, Value>::size_type
Trie<Key, Value>::erase(const key_type &k)
{
    //todo
    return 0;
}

template <class Key, class Value>
typename Trie<Key, Value>::mapped_type&
Trie<Key, Value>::operator[] (const key_type& k)
{
    // TODO: need iterator first
    //return (*((insert(make_pair(k,mapped_type()))).first)).second;
}

template <class Key, class Value>
typename Trie<Key, Value>::size_type Trie<Key, Value>::count(const key_type& k) const
{
    //todo
}

template <class Key, class Value>
void Trie<Key, Value>::clear()
{
}

template <class Key, class Value>
typename Trie<Key, Value>::node_type * Trie<Key, Value>::clearSubTree(node_type *n)
{
    return nullptr;
    //todo
}
#endif /* SQUID_TERNARYTRIE_H_ */

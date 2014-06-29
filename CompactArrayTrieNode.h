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
    // return a pointer to the stored value of the longest-matching-prefix
    // (if prefix==true) or exact match; return NULL if nothing is found.
    CompactArrayTrieNode *find (Key const &, size_t pos = 0, bool const prefix = false) const;

    // add a new value_type made of Key and Value in the position pointed
    // to by Key. This method is meant to be called on the root node of the
    // Trie; will recurse on the internal variant by the same name.
    // returns false if the string can't be added.
    // will overwrite previously-set data with the same key
    bool add (Key const &, Value &);

private:
    typedef std::vector<CompactArrayTrieNode *> children_t;

    children_t children;
    value_type data;
    int offset;
    bool haveData;

    /* not implemented */
    CompactArrayTrieNode(const CompactArrayTrieNode&);
    CompactArrayTrieNode& operator= (CompactArrayTrieNode const &);

    bool add (Key const &, Value &, size_t pos);
};

template <class Key, class Value>
CompactArrayTrieNode<Key,Value>::CompactArrayTrieNode() :
    offset(0), haveData(false)
{}

template <class Key, class Value>
CompactArrayTrieNode<Key,Value>::~CompactArrayTrieNode()
{
    for (typename children_t::iterator i=children.begin(); i != children.end(); ++i)
        delete *i; // handles nullptr automatically
    // no need to handle data type, it has by-value semantics
}


template <class Key, class Value>
CompactArrayTrieNode<Key,Value> *
CompactArrayTrieNode<Key,Value>::find (Key const & key, size_t pos, bool const prefix) const
{
    if (pos < key.size()) {
        // todo: charTransform?
        int character = key[pos];
        int realPos = -1;

        if (character >= offset && character - offset < children.size() )
            realPos = character - offset;
        if (realPos >= 0 && children[realPos]) {
            value_type *result = children[realPos]->find(key, pos+1, prefix);
            if (result)
                return result;
        }
        if (prefix && haveData)
            return this;
        return NULL;
    } else {
        if (haveData)
            return this;
        return nullptr;
    }
}

template <class Key, class Value>
bool
CompactArrayTrieNode<Key,Value>::add(Key const &k , Value &v)
{
    return add(k,v,0);
}

template <class Key, class Value>
bool
CompactArrayTrieNode<Key,Value>::add(Key const &k , Value &v, size_t pos)
{
    // terminal node in the string. Add data here.
    if (pos == k.size()) {
        haveData = true;
        data = std::make_pair(k,v);
        return true;
    }
    const size_t slot = k[pos];
    if (children.empty()) { // empty children. Make room for exactly one
        children.resize(1, nullptr);
        offset = slot;
    } else if ( slot < offset) { //underflow. Insert and shift at head
        children.insert(children.begin(), offset - slot, nullptr);
        offset = slot;
    } else if (slot >= offset + children.size()) { //overflow. extend children
        children.resize(slot - offset + 1, nullptr);
    }
    const size_t actual_slot = slot - offset;
    if (!children[actual_slot])
        children[actual_slot] = new CompactArrayTrieNode;
    return children[actual_slot]->add(k,v,pos+1);
}

#endif /* SQUID_COMPACTARRAYTRIENODE_H_ */

#ifndef SQUID_COMPACTARRAYTRIENODE_H_
#define SQUID_COMPACTARRAYTRIENODE_H_

#include <vector>

template <class Key, class Value>
class CompactTrie;

template <class Key, class Value>
class CompactArrayTrieNode
{
public:
    typedef unsigned int size_t;
    typedef Key key_type;
    typedef Value mapped_type;
    typedef std::pair<key_type, mapped_type> value_type;
    CompactArrayTrieNode();
    ~CompactArrayTrieNode();

    // return a pointer to the node corresponding to the char, or nullptr
    // if not found
    CompactArrayTrieNode *find (int character);

    // return a pointer to the node keyed on key, if found, or nullptr
    // if no exact key matches
    CompactArrayTrieNode *recursiveFind (Key const & k) {
        return recursiveLowFind(k, 0, false, false, 0);
    }

    // return a pointer to the node keyed on the SHORTEST prefix of key
    // or nullptr if no prefix is found
    CompactArrayTrieNode *recursivePrefixFind (Key const & k) {
        return recursiveLowFind(k, 0, true, false, 0);
    }

    // return a pointer to the node keyed on the SHORTEST prefix of key
    // ending with the supplied suffix char or nullptr if none is found
    CompactArrayTrieNode *recursivePrefixFind (Key const & k, int suffixChar) {
        return recursiveLowFind(k, 0, true, true, suffixChar);
    }

    // add a new value_type made of Key and Value in the position pointed
    // to by Key. This method is meant to be called on the root node of the
    // Trie; will recurse on the internal variant by the same name.
    // returns false if the string can't be added.
    // will overwrite previously-set data with the same key
    bool recursiveAdd (key_type const &, mapped_type);

    friend class CompactTrie<key_type, mapped_type>;

private:
    // return a pointer to the stored value of the longest-matching-prefix
    // (if prefix==true) or exact match; return NULL if nothing is found.
    // if havetrailchar is true, then a prefix is matched only if it ends
    // wuth the specified trailchar, otherwise it is ignored.
    CompactArrayTrieNode *recursiveLowFind (Key const &, size_t pos, bool const prefix, bool haveTrailChar, int trailchar);

    typedef std::vector<CompactArrayTrieNode *> children_type;

    children_type children;
    value_type data;
    int offset;
    bool haveData;

    /* not implemented */
    CompactArrayTrieNode(const CompactArrayTrieNode&);
    CompactArrayTrieNode& operator= (CompactArrayTrieNode const &);

    bool recursiveAdd (key_type const &, mapped_type &, size_t pos);
};

template <class key_type, class mapped_type>
CompactArrayTrieNode<key_type,mapped_type>::CompactArrayTrieNode() :
    offset(0), haveData(false)
{}

template <class key_type, class mapped_type>
CompactArrayTrieNode<key_type,mapped_type>::~CompactArrayTrieNode()
{
    for (typename children_type::iterator i=children.begin(); i != children.end(); ++i)
        delete *i; // handles nullptr automatically
    // no need to handle data type, it has by-value semantics
}

template <class key_type, class mapped_type>
CompactArrayTrieNode<key_type,mapped_type> *
CompactArrayTrieNode<key_type,mapped_type>::recursiveLowFind (key_type const & key, size_t pos, bool const prefix, bool haveTrailChar, int trailchar)
{
    if (pos < key.size()) {
        // todo: charTransform?
        int character = key[pos];

        if (prefix && !haveTrailChar && haveData)
            return this;
        if (haveTrailChar && character == trailchar) {
            CompactArrayTrieNode *child = find(character);
            if (child->haveData)
                return child;
        }

        const CompactArrayTrieNode *child = find(character);
        CompactArrayTrieNode *result = nullptr;
        if (child)
             result = find(character)->recursiveLowFind(key, pos+1, prefix, haveTrailChar, trailchar);
        if (result) {
            return result;
        }
        return nullptr;
    } else {
        if (haveData) {
            return this;
        }
        // special case: if prefix and haveTrailChar, we look up one more level
        // for the trail char
        CompactArrayTrieNode *child;
        if (prefix && haveTrailChar) {
            child = find(trailchar);
            if (child != nullptr && child->haveData)
                return child;
        }
        return nullptr;
    }
}

template <class key_type, class mapped_type>
CompactArrayTrieNode<key_type,mapped_type> *
CompactArrayTrieNode<key_type,mapped_type>::find (int character)
{
    int realPos = -1;
    if (character >= offset && character - offset < children.size() )
        realPos = character - offset;
    if (realPos >= 0 && children[realPos])
        return children[realPos];
    return nullptr;
}

template <class key_type, class mapped_type>
bool
CompactArrayTrieNode<key_type,mapped_type>::recursiveAdd(key_type const &k , mapped_type v)
{
    return recursiveAdd(k,v,0);
}

template <class key_type, class mapped_type>
bool
CompactArrayTrieNode<key_type,mapped_type>::recursiveAdd(key_type const &k , mapped_type &v, size_t pos)
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
    return children[actual_slot]->recursiveAdd(k,v,pos+1);
}

#endif /* SQUID_COMPACTARRAYTRIENODE_H_ */

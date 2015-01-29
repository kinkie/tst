#ifndef SQUID_COMPACTARRAYTRIENODE_H_
#define SQUID_COMPACTARRAYTRIENODE_H_

#include <vector>

template <class Key, class Value>
class CompactTrie;
template <class Key, class Value>
class CompactTrieIterator;

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
    CompactArrayTrieNode *find(int character);

    // return a pointer to the node keyed on key, if found, or nullptr
    // if no exact key matches
    CompactArrayTrieNode *recursiveFind(Key const & k) {
        return recursiveLowFind(k, 0, false, false, 0);
    }

    // return a pointer to the node keyed on the SHORTEST prefix of key
    // or nullptr if no prefix is found
    CompactArrayTrieNode *recursivePrefixFind(Key const & k) {
        return recursiveLowFind(k, 0, true, false, 0);
    }

    // return a pointer to the node keyed on the SHORTEST prefix of key
    // ending with the supplied suffix char or nullptr if none is found
    CompactArrayTrieNode *recursivePrefixFind(Key const & k, int const suffixChar) {
        return recursiveLowFind(k, 0, true, true, suffixChar);
    }

    // true if the subtree (including the current node) is empty
    // NOTE: this implementation is only correct if it's not possible
    // to remove children from the TrieNode. Once that API is implemented,
    // in order for empty() to be correct, it'll have to also clear the
    // children array once the last child is removed.
    bool empty() const {
        return (!haveData && children.empty());
    }

    // add a new value_type made of Key and Value in the position pointed
    // to by Key. This method is meant to be called on the root node of the
    // Trie; will recurse on the internal variant by the same name.
    // returns false if the string can't be added.
    // will overwrite previously-set data with the same key
    bool recursiveAdd(key_type const &, mapped_type);

    // walk the subtree and fill the passed std::vector with pointers to nodes having data
    void recursivePreorderWalk(std::vector<CompactArrayTrieNode *> &) ;

    friend class CompactTrie<key_type, mapped_type>;
    friend class CompactTrieIterator<key_type, mapped_type>;

private:
    // return a pointer to the stored value of the longest-matching-prefix
    // (if prefix==true) or exact match; return NULL if nothing is found.
    // if havetrailchar is true, then a prefix is matched only if it ends
    // wuth the specified trailchar, otherwise it is ignored.
    CompactArrayTrieNode *recursiveLowFind(Key const &, size_t pos, bool const prefix, bool const haveTrailChar, int const trailchar);

    typedef std::vector<CompactArrayTrieNode *> children_type;

    children_type children;
    value_type data;
    int offset;
    bool haveData;

    /* not implemented */
    CompactArrayTrieNode(const CompactArrayTrieNode&);
    CompactArrayTrieNode& operator =(CompactArrayTrieNode const &);

    bool recursiveAdd(key_type const &, const mapped_type &, size_t pos);
    static bool iterativeAdd(const key_type &, const mapped_type &, CompactArrayTrieNode *);
};

template <class key_type, class mapped_type>
CompactArrayTrieNode<key_type,mapped_type>::CompactArrayTrieNode() :
        offset(0),
        haveData(false)
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
CompactArrayTrieNode<key_type,mapped_type>::recursiveLowFind(key_type const & key, size_t pos, bool const prefix, const bool haveTrailChar, const int trailchar)
{
    if (pos < key.size()) {

        // not yet at the end of the key string, grab the next character
        const int character = key[pos];

        // the tree entry ending here is prefix of key, no need to search further
        if (prefix && !haveTrailChar && haveData)
            return this;

        // does key character have any data associated to search in?
        CompactArrayTrieNode *child = find(character);

        // if the key is "moc.elpmaxe.www" and tree contains "moc.elpmaxe." we want a match.
        // "moc.elpmaxe." is a tree entry when child('.')->haveData == true
        //
        // NP: check for this here instead of on recurse because the child node
        //     does not 'know' what character we used to reach it.
        if (prefix && haveTrailChar && character == trailchar && child && child->haveData)
            return child;

        CompactArrayTrieNode *result = nullptr;

        // recurse if there is a child sub-tree
        if (child)
            result = child->recursiveLowFind(key, pos+1, prefix, haveTrailChar, trailchar);

        return result; // may be nullptr

    } else {
        // reached end of the key string, when a tree entry also ends here no need to continue.
        if (haveData)
            return this;

        // if the key is "moc.elpmaxe" and tree contains "moc.elpmaxe." we want a match.
        // - "moc.elpmaxe." is a tree entry only if child('.')->haveData == true
        CompactArrayTrieNode *child = nullptr;
        if (prefix && haveTrailChar) {
            child = find(trailchar);
            if (child && !child->haveData)
                return nullptr;
        }
        return child; // may be nullptr
    }
}

template <class key_type, class mapped_type>
CompactArrayTrieNode<key_type,mapped_type> *
CompactArrayTrieNode<key_type,mapped_type>::find(int character)
{
    const int realPos = character - offset;
    if (character >= offset && realPos < children.size()) {
        if (children[realPos])
            return children[realPos];
    }
    return nullptr;
}

template <class key_type, class mapped_type>
bool
CompactArrayTrieNode<key_type,mapped_type>::recursiveAdd(key_type const &k , mapped_type v)
{
    //return recursiveAdd(k,v,0);
    return iterativeAdd(k, v, this);
}

template <class key_type, class mapped_type>
bool
CompactArrayTrieNode<key_type,mapped_type>::recursiveAdd(const key_type &k, const mapped_type &v, size_t pos)
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
    } else if (slot < offset) { //underflow. Insert and shift at head
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

template <class key_type, class mapped_type>
bool
CompactArrayTrieNode<key_type,mapped_type>::iterativeAdd(const key_type &k, const mapped_type &v, CompactArrayTrieNode *n)
{
    auto e = k.end();
    auto i=k.begin();
    while (i != e) {
        const size_t slot = *i;
        if (n->children.empty()) { // empty children. Make room for exactly one
            n->children.resize(1, nullptr);
            n->offset = slot;
        } else if (slot < n->offset) { //underflow. Insert and shift at head
            n->children.insert(n->children.begin(), n->offset - slot, nullptr);
            n->offset = slot;
        } else if (slot >= n->offset + n->children.size()) { //overflow. extend children
            n->children.resize(slot - n->offset + 1, nullptr);
        }
        const size_t actual_slot = slot - n->offset;
        if (!n->children[actual_slot])
            n->children[actual_slot] = new CompactArrayTrieNode;
        n = n->children[actual_slot];
        ++i;
    }
    n->haveData = true;
    n->data = std::make_pair(k,v);
    return true;
}

template <class key_type, class mapped_type>
void
CompactArrayTrieNode<key_type,mapped_type>::recursivePreorderWalk(std::vector<CompactArrayTrieNode *> &v)
{
    if (haveData)
        v.push_back(this);
    for (auto c = children.begin(); c != children.end(); ++c) {
        if (*c)
            (*c)->recursivePreorderWalk(v);
    }
}

#endif /* SQUID_COMPACTARRAYTRIENODE_H_ */

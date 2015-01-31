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
    CompactArrayTrieNode *findInNode(int character);

    // return a pointer to the node keyed on key, if found, or nullptr
    // if no exact key matches
    CompactArrayTrieNode *find(Key const & k) {
        return iterativeLowFind(k, false, false, 0, this);
    }

    // return a pointer to the node keyed on the SHORTEST prefix of key
    // or nullptr if no prefix is found
    CompactArrayTrieNode *findPrefix(Key const & k) {
        return iterativeLowFind(k, true, false, 0, this);
    }

    // return a pointer to the node keyed on the SHORTEST prefix of key
    // ending with the supplied suffix char or nullptr if none is found
    CompactArrayTrieNode *findPrefix(Key const & k, int const suffixChar) {
        return iterativeLowFind(k, true, true, suffixChar, this);
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
    bool insert(key_type const &, mapped_type);

    // walk the subtree and fill the passed std::vector with pointers to nodes having data
    void recursivePreorderWalk(std::vector<CompactArrayTrieNode *> &) ;

    friend class CompactTrie<key_type, mapped_type>;
    friend class CompactTrieIterator<key_type, mapped_type>;

private:
    // return a pointer to the stored value of the longest-matching-prefix
    // (if prefix==true) or exact match; return NULL if nothing is found.
    // if havetrailchar is true, then a prefix is matched only if it ends
    // wuth the specified trailchar, otherwise it is ignored.
    static CompactArrayTrieNode *iterativeLowFind(Key const &, bool const prefix, bool const haveTrailChar, int const trailchar, CompactArrayTrieNode *n);

    typedef std::vector<CompactArrayTrieNode *> children_type;

    children_type children;
    value_type data;
    int offset;
    bool haveData;

    /* not implemented */
    CompactArrayTrieNode(const CompactArrayTrieNode&);
    CompactArrayTrieNode& operator =(CompactArrayTrieNode const &);

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
CompactArrayTrieNode<key_type,mapped_type>::iterativeLowFind(key_type const & key, bool const prefix, const bool haveTrailChar, const int trailchar, CompactArrayTrieNode *n)
{
    auto end = key.end();
    auto i = key.begin();

    while (i != end) {
        // not yet at the end of the key string, grab the next character
        const int character = *i;

        // the tree entry ending here is prefix of key, no need to search further
        if (prefix && !haveTrailChar && n->haveData)
            return n;

        // does key character have any data associated to search in?
        CompactArrayTrieNode *child = n->findInNode(character);

        // if the key is "moc.elpmaxe.www" and tree contains "moc.elpmaxe." we want a match.
        // "moc.elpmaxe." is a tree entry when child('.')->haveData == true
        //
        // NP: check for this here instead of on recurse because the child node
        //     does not 'know' what character we used to reach it.
        if (prefix && haveTrailChar && character == trailchar && child && child->haveData)
            return child;

        // if we have a child, iterate into it, otherwise it's a miss
        if (child) {
            n = child;
            ++i;
        } else {
            return nullptr;
        }
    }
    // i == end, whole key was matched
    if (n->haveData)
        return n;

    // if the key is "moc.elpmaxe" and tree contains "moc.elpmaxe." we want a match.
    // - "moc.elpmaxe." is a tree entry only if child('.')->haveData == true
    if (prefix && haveTrailChar) {
        const auto child = n->findInNode(trailchar);
        if (child && child->haveData)
            return child;
        return nullptr;
    }

    return nullptr;
}

template <class key_type, class mapped_type>
CompactArrayTrieNode<key_type,mapped_type> *
CompactArrayTrieNode<key_type,mapped_type>::findInNode(int character)
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
CompactArrayTrieNode<key_type,mapped_type>::insert(key_type const &k , mapped_type v)
{
    return iterativeAdd(k, v, this);
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

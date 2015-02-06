#ifndef SQUID_COMPACTARRAYTRIENODE_H_
#define SQUID_COMPACTARRAYTRIENODE_H_

#include <vector>

template <class Key, class Value>
class CompactTrie;
template <class Key, class Value>
class CompactTrieIterator;

/** Private auxiliary class for CompactTrie.
 *
 * DO NOT USE or try to access it in any other context.
 */
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

    /** node lookup
     *
     * Main nonrecursive node lookup funciton.
     * \return pointer to child node corresponding to char if present,
     *  nullptr if not present.
     */
    CompactArrayTrieNode *findInNode(int character);

    /** subtree lookup
     *
     * \return a pointer to the node containing the exact key match if found,
     *  or nullptr if the key is not contained in the subtree
     */
    CompactArrayTrieNode *find(Key const & k) {
        return find(k.begin(), k.end());
    }
    /// subtree lookup, iterator-based variant
    template <class InputIterator>
    CompactArrayTrieNode *find(InputIterator begin, const InputIterator& end) {
        return iterativeLowFind(begin, end, false, false, 0, this);
    }

    /** subtree prefix lookup
     *
     * \return pointer to the node keyed on the SHORTEST prefix of key
     *   or nullptr if no prefix is found
     */
    CompactArrayTrieNode *findPrefix(Key const & key) {
        return findPrefix(key.begin(), key.end());
    }
    /// subtree prefix lookup, iterator-based variant
    template <class InputIterator>
    CompactArrayTrieNode *findPrefix(InputIterator begin, const InputIterator& end) {
        return iterativeLowFind(begin, end, true, false, 0, this);
    }

    /** subtree prefix lookup with terminator constraints
     *
     * Search for the node corresponding to the shortest prefix of the supplied
     * key where the prefix ends with the supplied suffixChar
     * or corresponding to the full key or to the full key plus suffixChar.
     * For example, findPrefix("foo%bar%gazonk",'%') will return a pointer
     * to the node corresponding to the first key among:
     * "foo%", "foo%bar%", "foo%bar%gazonk", "foo%bar%gazonk%", nullptr
     *
     * \return pointer to node if any constrained prefix is found, nullptr
     *   if no such prefix is found
     */
    CompactArrayTrieNode *findPrefix(Key const & key, int const suffixChar) {
        return findPrefix(key.begin(), key.end(), suffixChar);
    }
    // subtree prefix lookup with terminator constraints, iterator variant
    template <class InputIterator>
    CompactArrayTrieNode *findPrefix(InputIterator begin, const InputIterator& end, int const suffixChar) {
        return iterativeLowFind(begin, end, true, true, suffixChar, this);
    }

    /** subtree emptyness
     *
     * \return true if the subtree (including the current node) is empty
     * \note this implementation is only correct if it's not possible
     *   to remove children from the TrieNode. Once that API is implemented,
     *   in order for empty() to be correct, it'll have to also clear the
     *   children array once the last child is removed.
     */
    bool empty() const {
        return (!haveData && children.empty());
    }

    /** insert a new value in the subtrie
     *
     * Add a new value_type made of Key and Value in the position pointed
     * to by Key. This method is meant to be called on the root node of the
     * Trie. Any preexisting value keyed on the same key gets replaced.
     *
     * \return false if the value can't be added.
     */
    bool insert(key_type const &k , const mapped_type &v) {
        return iterativeAdd(k.begin(), k.end(), k, v, this);
    }

    /** data collector
     *
     * walk the subtree and fill the passed std::vector reference
     * with pointers to nodes having data, sorted lexicographically
     */
    void recursivePreorderWalk(std::vector<CompactArrayTrieNode *> &) ;

    friend class CompactTrie<key_type, mapped_type>;
    friend class CompactTrieIterator<key_type, mapped_type>;

private:
    /** low-level matching method
     *
     * Low-level function implementing all the find variants: exact key match
     *  (if prefix == false), prefix match (if prefix == true) and constrained
     *  prefix metch (if prefix == true && haveTrailChar == true)
     * \return pointer sought-for node or nullptr if not found.
     */
    template <class InputIterator>
    static CompactArrayTrieNode *iterativeLowFind(InputIterator begin, const InputIterator &end, bool const prefix, bool const haveTrailChar, int const trailchar, CompactArrayTrieNode *n);

    typedef std::vector<CompactArrayTrieNode *> children_type;

    children_type children;
    value_type data;
    int offset;
    bool haveData;

    /// not implemented
    CompactArrayTrieNode(const CompactArrayTrieNode&);
    /// not implemented
    CompactArrayTrieNode& operator =(CompactArrayTrieNode const &);

    /** low-level data insert
     *
     * Low-level function inserting new data in the Trie (or replacing
     * data with the same key). Extends Trie storage as needed.
     * \return false if data cannot be added
     */
    static bool iterativeAdd(const key_type &, const mapped_type &, CompactArrayTrieNode *);
    template <class InputIterator>
    /// low-level data insert, iterator-based variant
    static bool iterativeAdd(InputIterator begin, const InputIterator &end, const key_type &, const mapped_type &, CompactArrayTrieNode *);
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
template <class InputIterator>
CompactArrayTrieNode<key_type,mapped_type> *
CompactArrayTrieNode<key_type,mapped_type>::iterativeLowFind(InputIterator i, const InputIterator &end, bool const prefix, const bool haveTrailChar, const int trailchar, CompactArrayTrieNode *n)
{
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
        // NP: check for this here instead of on iterate because the child node
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

// not used anymore; kept around as a reference for now
template <class key_type, class mapped_type>
bool
CompactArrayTrieNode<key_type,mapped_type>::iterativeAdd(const key_type &k, const mapped_type &v, CompactArrayTrieNode *n)
{
    return iterativeAdd(k.begin(), k.end(), k, v, n);
}

template <class key_type, class mapped_type>
template <class InputIterator>
bool
CompactArrayTrieNode<key_type,mapped_type>::iterativeAdd(InputIterator i, const InputIterator &end, const key_type &k,const mapped_type &v, CompactArrayTrieNode *n)
{
    while (i != end) {
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

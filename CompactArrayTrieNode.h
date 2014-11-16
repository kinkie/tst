#ifndef SQUID_COMPACTARRAYTRIENODE_H_
#define SQUID_COMPACTARRAYTRIENODE_H_

#include <iterator>
#include <vector>
#include <stack>

template <class Key, class Value>
class CompactArrayTrieBaseNodeIterator;
template <class Key, class Value>
class CompactArrayTrieNodeIterator;

template <class Key, class Value>
class CompactArrayTrieNode
{
public:
    typedef std::pair<Key,Value> value_type;
    CompactArrayTrieNode();
    ~CompactArrayTrieNode();

    // return a pointer to the node corresponding to the char, or nullptr
    // if not found
    CompactArrayTrieNode *find (int character);

    // return a pointer to the stored value of the longest-matching-prefix
    // (if prefix==true) or exact match; return NULL if nothing is found.
    CompactArrayTrieNode *recursiveFind (Key const &, size_t pos = 0, bool const prefix = false);

    // add a new value_type made of Key and Value in the position pointed
    // to by Key. This method is meant to be called on the root node of the
    // Trie; will recurse on the internal variant by the same name.
    // returns false if the string can't be added.
    // will overwrite previously-set data with the same key
    bool recursiveAdd (Key const &, Value);

    typedef CompactArrayTrieNodeIterator<Key,Value> iterator;
    CompactArrayTrieNodeIterator<Key,Value> begin() {
        return iterator(this, children.begin());
    }
    CompactArrayTrieNodeIterator<Key,Value> end() {
        return iterator(this, children.end());
    }

private:
    typedef std::vector<CompactArrayTrieNode *> children_type;

    children_type children;
    value_type data;
    int offset;
    bool haveData;

    /* not implemented */
    CompactArrayTrieNode(const CompactArrayTrieNode&);
    CompactArrayTrieNode& operator= (CompactArrayTrieNode const &);

    bool recursiveAdd (Key const &, Value &, size_t pos);
    friend class CompactArrayTrieBaseNodeIterator<Key,Value>;
    friend class CompactArrayTrieNodeIterator<Key,Value>;
};

template <class Key, class Value>
CompactArrayTrieNode<Key,Value>::CompactArrayTrieNode() :
    offset(0), haveData(false)
{}

template <class Key, class Value>
CompactArrayTrieNode<Key,Value>::~CompactArrayTrieNode()
{
    for (typename children_type::iterator i=children.begin(); i != children.end(); ++i)
        delete *i; // handles nullptr automatically
    // no need to handle data type, it has by-value semantics
}

template <class Key, class Value>
CompactArrayTrieNode<Key,Value> *
CompactArrayTrieNode<Key,Value>::recursiveFind (Key const & key, size_t pos, bool const prefix)
{
    if (pos < key.size()) {
        // todo: charTransform?
        int character = key[pos];
        const CompactArrayTrieNode *child = find(character);
        CompactArrayTrieNode *result = nullptr;
        if (child)
             result = find(character)->recursiveFind(key, pos+1, prefix);
        if (result)
            return result;
        if (prefix && haveData)
            return this;
        return nullptr;
    } else {
        if (haveData)
            return this;
        return nullptr;
    }
}

template <class Key, class Value>
CompactArrayTrieNode<Key,Value> *
CompactArrayTrieNode<Key,Value>::find (int character)
{
    int realPos = -1;
    if (character >= offset && character - offset < children.size() )
        realPos = character - offset;
    if (realPos >= 0 && children[realPos])
        return children[realPos];
    return nullptr;
}


template <class Key, class Value>
bool
CompactArrayTrieNode<Key,Value>::recursiveAdd(Key const &k , Value v)
{
    return recursiveAdd(k,v,0);
}

template <class Key, class Value>
bool
CompactArrayTrieNode<Key,Value>::recursiveAdd(Key const &k , Value &v, size_t pos)
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

// nonrecursive iterator. iterates through pointers to the children of a node
template <class Key, class Value>
class CompactArrayTrieBaseNodeIterator
                : public std::iterator<std::forward_iterator_tag, CompactArrayTrieNode<Key,Value> >
{
    typedef typename CompactArrayTrieNode<Key,Value>::children_type::iterator base_type;
    CompactArrayTrieNode<Key,Value> *node;
    base_type childrenIter;

    /* no default constructor */
    CompactArrayTrieBaseNodeIterator();
public:
    // only public constructor, taking a pointer to a CompactArrayTrieNode (for begin and end) and
    // to a forward iterator which supplies the underlying implementation
    CompactArrayTrieBaseNodeIterator(CompactArrayTrieNode<Key,Value>* n, base_type i) :
        childrenIter(i), node(n) {}
    
    // equality tests
    bool operator ==(const CompactArrayTrieBaseNodeIterator& i) {
        return childrenIter==i.childrenIter;
    }
    bool operator !=(const CompactArrayTrieBaseNodeIterator& i) {
        return !operator==(i);
    }
    // increment operator. Calls the base class' increment, skipping over empty children
    CompactArrayTrieBaseNodeIterator& operator++() {
        while (childrenIter != node->end() && *childrenIter == nullptr)
            ++childrenIter;
        return this;
    }
    // dereference
    typename CompactArrayTrieNode<Key,Value>::children_type operator *() {
        return *childrenIter;
    }
};

template <class Key, class Value>
class CompactArrayTrieNodeIterator
                : public std::iterator<std::forward_iterator_tag, CompactArrayTrieNode<Key,Value> >
{
    typedef std::stack<CompactArrayTrieBaseNodeIterator<Key,Value> > iter_state_type;
    typedef typename CompactArrayTrieNode<Key,Value>::children_type::iterator base_type;

    iter_state_type iter_state;

    /* no default constructor */
    CompactArrayTrieNodeIterator();

public:
    CompactArrayTrieNodeIterator(CompactArrayTrieNode<Key,Value>* n, base_type i)
    {
        iter_state.push(CompactArrayTrieBaseNodeIterator<Key,Value>(n,i));
    }

    // equality tests
    bool operator ==(const CompactArrayTrieNodeIterator& i) {
        return iter_state.top() == i.iter_state.top();
    }
    bool operator !=(const CompactArrayTrieNodeIterator& i) {
        return !operator==(i);
    }

};
#endif /* SQUID_COMPACTARRAYTRIENODE_H_ */

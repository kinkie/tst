#ifndef SQUID_TERNARYTRIE_H_
#define SQUID_TERNARYTRIE_H_

#include <stdexcept>
#include <utility>

/* for inspiration see:
 * https://github.com/nlehuen/pytst/tree/master/include
 * https://code.google.com/p/tstdb/source/browse/#svn%2Ftrunk%2Fv2%2Fsrc
 * http://algs4.cs.princeton.edu/52trie/TST.java.html
 */
template <class Key, class Value>
class TernaryTrie
{
public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef std::pair<Key, Value> value_type;
    typedef unsigned int size_type;

    class Node {
        friend class TernaryTrie;
        Node *left_, *right_, *equal_, *parent_;
        TernaryTrie::value_type data; // if data.first.empty(), no data is present
        unsigned char c;
    public:
        Node ( Node *parent, unsigned char c_ ) : left_(nullptr), right_(nullptr), equal_(nullptr), parent_(parent), c(c_) {}
        ~Node();
    };

    class TernaryTrieIterator {
    public:
    };

    typedef TernaryTrieIterator iterator;
    typedef const TernaryTrieIterator const_iterator;
    // typedef ... reverse_iterator
    // typedef ... const_reverse_iterator

    TernaryTrie();
    ~TernaryTrie();

    TernaryTrie& operator  = (const TernaryTrie &);

    // iterator begin();
    // const_iterator cbegin() const;
    // iterator end();
    // const_iterator cend() const;
    // reverse_iterator rbegin();
    // const_reverse_iterator crbegin() const;
    // reverse_iterator rend();
    // const_reverse_iterator crend() const;

    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type max_size() const; // placeholder

    mapped_type& operator[] (const key_type& k); //(*((this->insert(make_pair(k,mapped_type()))).first)).second
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

    //size_type count (const key_type& k) const { return find(k)!=end(); }

private:
    // return a Node* or nullptr if key is not in the tree
    Node *getNode(Node *subtree, const key_type &key, size_type pos = 0);
    Node *putNode(Node *n, const value_type& value, int depth = 0);

    Node *parent(Node *n) const { if (n != nullptr) return n.parent_; return nullptr; }
    Node *left(Node *n) const { if (n != nullptr) return n->left_; return nullptr; }
    Node *equal(Node *n) const { if (n != nullptr) return n->equal_; return nullptr; }
    Node *right(Node *n) const { if (n != nullptr) return n->right_; return nullptr; }

    Node *root;
    size_type size_;
    uint32_t version;
    bool readonly;
};

template <class Key, class Value>
TernaryTrie<Key, Value>::TernaryTrie() : root(nullptr), size_(0), version(0), readonly(false) { }

template <class Key, class Value>
TernaryTrie<Key, Value>::~TernaryTrie() {
    delete root;
}

template <class Key, class Value>
TernaryTrie<Key, Value>::Node::~Node() {
    delete left_;
    delete right_;
    delete equal_;
}

template <class Key, class Value>
const typename TernaryTrie<Key, Value>::mapped_type& TernaryTrie<Key, Value>::at (const key_type& k)
const throw (std::out_of_range)
{
    if (!root)
        throw std::out_of_range("Element not found");
    Node * n=getNode(root,k);
    if (n && n->data.first.size())
        return n->data.second;
    throw std::out_of_range("Element not found");
}

template <class Key, class Value>
typename TernaryTrie<Key, Value>::mapped_type& TernaryTrie<Key, Value>::at (const key_type& k)
throw (std::out_of_range)
{
    if (!root)
        throw std::out_of_range("Element not found");
    Node * n = getNode(root,k);
    if (n && n->data.first.size())
        return n->data.second;
    throw std::out_of_range("Element not found");
}

template <class Key, class Value>
typename TernaryTrie<Key, Value>::Node *
TernaryTrie<Key, Value>::getNode (Node *subtree, const key_type &key, size_type pos)
{
    if (subtree == nullptr)
        return nullptr;
    if (key[pos] < subtree->c)
        return getNode(left(subtree), key, pos); // will return nullptr if subtree doesn't exist
    if (key[pos] > subtree->c)
        return getNode(right(subtree), key, pos);  // will return nullptr if subtree doesn't exist
    if (pos < key.size()-1)
        return getNode(equal(subtree), key, pos+1);
    return subtree;
}

template <class Key, class Value>
typename TernaryTrie<Key, Value>::Node *TernaryTrie<Key, Value>::putNode (
                Node *n,
                const value_type &v,
                int depth)
{
    char c=v.first[depth];
    if (n == nullptr) {
        n = new Node(nullptr,c);
    }
    if (c < n->c)
        n->left_ = putNode(n->left_, v, depth);
    else if (c > n->c)
        n->right_ = putNode(n->right_, v, depth);
    else if (depth < v.first.size() -1)
        n->equal_ = putNode(n->equal_, v, depth+1);
    else
        n->data = v;
    return n;
}

template <class Key, class Value>
typename std::pair<typename TernaryTrie<Key, Value>::iterator, bool>
TernaryTrie<Key, Value>::insert(const value_type &val)
{
    // todo: increment size.
    // todo: enforce readonly
    root = putNode(root, val);
    return std::make_pair(iterator(), true); //todo: return a real iterator
}

template <class Key, class Value>
template <class InputIterator>
void
TernaryTrie<Key, Value>::insert(InputIterator first, InputIterator last)
{
    for (InputIterator i = first; i != last; ++i)
        insert(*i);
}

template <class Key, class Value>
typename TernaryTrie<Key, Value>::size_type
TernaryTrie<Key, Value>::erase(const key_type &k)
{
    Node *n=getNode(root, k);
    if (!n)
        return;
    if (n->data.first.size())
        n->data = value_type();
}
#endif /* SQUID_TERNARYTRIE_H_ */

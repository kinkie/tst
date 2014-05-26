#ifndef SQUID_TERNARYTRIE_H_
#define SQUID_TERNARYTRIE_H_

#include <utility>

template <class Key, class Value>
class TernaryTrie
{
public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef std::pair<const Key, Value> value_type;
    typedef std::size_t size_type;

    class Node {
        Node *left_, *right_, *equal_, *parent_;
        TernaryTrie::value_type data;
        unsigned char c;
    public:
        explicit Node ( Node *parent );
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

    bool empty() const;
    size_type size() const;
    size_type max_size() const;

    mapped_type& operator[] (const key_type& k); //(*((this->insert(make_pair(k,mapped_type()))).first)).second
    mapped_type& at (const key_type& k);
    const mapped_type& at (const key_type& k) const;

    std::pair<iterator,bool> insert (const value_type& val);
    template <class InputIterator>
    void insert (InputIterator first, InputIterator last);

    //void erase (iterator position);
    size_type erase (const key_type& k);
    //void erase (iterator first, iterator last);

    //void swap (map& x);
    void clear();

    //iterator find (const key_type& k);
    //const_iterator find (const key_type& k) const;
    //iterator find_longest_prefix (const key_type& k);
    //const_iterator find_longest_prefix (const key_type& k) const;

    //size_type count (const key_type& k) const { return find(k)!=end(); }
};

#endif /* SQUID_TERNARYTRIE_H_ */

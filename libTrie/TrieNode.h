/*
 * Copyright (c) 2002,2003 Robert Collins <rbtcollins@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 */

#ifndef   LIBTRIE_TRIENODE_H
#define   LIBTRIE_TRIENODE_H

#include "TrieCharTransform.h"

#include <sys/types.h>
#include <utility>

/* TODO: parameterize this to be more generic -
* i.e. M-ary internal node sizes etc
*/

class ArrayTrieNode
{

public:
    ArrayTrieNode();
    ~ArrayTrieNode();
    ArrayTrieNode(ArrayTrieNode const &);
    ArrayTrieNode &operator= (ArrayTrieNode const &);

    /* Find a string.
    * If found, return the private data.
    * If not found, return NULL.
    */
    inline void *find (char const *, size_t, TrieCharTransform *, bool const prefix) const;

    /* Add a string.
    * returns false if the string is already
    * present or can't be added.
    */

    bool add (char const *, size_t, void *, TrieCharTransform *);

private:
    /* 256-way Trie */
    /* The char index into internal is an
    * 8-bit prefix to a string in the trie.
    * internal[0] is the terminal node for
    * a string and may not be used
    */
    ArrayTrieNode * internal[256];

    /* If a string ends here, non NULL */
    void *_privateData;
};

class CompactArrayTrieNode {
public:
    CompactArrayTrieNode();
    ~CompactArrayTrieNode();
    inline void *find (char const *, size_t, TrieCharTransform *, bool const prefix) const;
    bool add (char const *, size_t, void *, TrieCharTransform *);
private:
    CompactArrayTrieNode(CompactArrayTrieNode const &);
    CompactArrayTrieNode& operator= (CompactArrayTrieNode const &);

    typedef std::vector<CompactArrayTrieNode*> internal_type;
    internal_type internal;
    void *_privateData;
    unsigned int offset; // ArrayTrieNode[j] is at CompactArrayTrieNode[offset+j]
};

/* recursive. TODO? make iterative */
void *
ArrayTrieNode::find (char const *aString, size_t theLength, TrieCharTransform *transform, bool const prefix) const
{
    if (theLength) {
        int index = -1;
        unsigned char pos = transform ? (*transform) (*aString) : *aString;

        if (internal[pos])
            index = pos;

        if (index > -1) {
            void *result = internal[index]->find(aString + 1, theLength - 1, transform, prefix);

            if (result)
                return result;
        }

        if (prefix)
            return _privateData;

        return NULL;
    } else {
        /* terminal node */
        return _privateData;
    }
}

#include <map>
class MapTrieNode {
public:
    MapTrieNode() : _privateData(NULL) {}
    inline ~MapTrieNode();
    MapTrieNode(MapTrieNode const &);
    MapTrieNode &operator=(MapTrieNode const &);

    inline void *find(char const *, size_t, TrieCharTransform *, bool const prefix) const;
    inline bool add (char const *, size_t, void *, TrieCharTransform *);

private:
    typedef std::map<unsigned char, MapTrieNode *> internalmap;
    internalmap internal;
    void *_privateData;
};

MapTrieNode::~MapTrieNode()
{
    using std::map;
    for (internalmap::iterator i =internal.begin(); i != internal.end(); ++i)
        if (i->second)
            delete i->second;
}

bool
MapTrieNode::add(char const *aString, size_t theLength, void *privatedata, TrieCharTransform *transform)
{
    if (theLength) {
        int index = transform ? (*transform)(*aString): *aString;
        const internalmap::iterator i = internal.find(index);

        if (i == internal.end())
            internal[index] = new MapTrieNode;

        return internal[index]->add(aString + 1, theLength - 1, privatedata, transform);
    } else {
        /* terminal node */

        if (_privateData)
            return false;

        _privateData = privatedata;

        return true;
    }
}

void *
MapTrieNode::find (char const *aString, size_t theLength, TrieCharTransform *transform, bool const prefix) const
{
    if (theLength) {
        unsigned char pos = transform ? (*transform) (*aString) : *aString;

        internalmap::const_iterator i = internal.find(pos);
        if (i != internal.end()) {
            void *result;
            result = i->second->find(aString + 1, theLength - 1, transform, prefix);

            if (result)
                return result;
        }

        if (prefix)
            return _privateData;

        return NULL;
    } else {
        /* terminal node */
        return _privateData;
    }

}

void *
CompactArrayTrieNode::find (char const *aString, size_t theLength, TrieCharTransform *transform, bool const prefix) const
{
    if (theLength) {
        unsigned char pos = transform ? (*transform) (*aString) : *aString;
        int realPos=-1;

        if (pos >= offset && pos - offset < internal.size() )
            realPos = pos - offset;
        if (realPos >= 0 && internal[realPos]) {
            void *result = internal[realPos]->find(aString + 1, theLength - 1, transform, prefix);
            if (result)
                return result;
        }
        if (prefix)
            return _privateData;
        return NULL;
    } else {
        return _privateData;
    }
}

#endif /* LIBTRIE_TRIENODE_H */

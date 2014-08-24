CXX=/usr/bin/g++
CFLAGS = -g
CXXFLAGS = -g -std=c++11
TESTS = TestCompactArrayTrieNode
#LIBS = libTernaryTrie.a

all: $(LIBS) check

check: $(TESTS)
	for a in $^; do ./$$a; done

clean:
	-rm $(LIBS) $(TESTS) *.o

#libTernaryTrie.a: TernaryTrie.o
#	ar cru $@ $^
#	
TestTernaryTrie: TestTernaryTrie.o Trie.h TernaryTrieNode.h
	g++ $(CXXFLAGS) $^ -o $@ -lcppunit

TestCompactArrayTrieNode: TestCompactArrayTrieNode.o CompactArrayTrieNode.h
	g++ $(CXXFLAGS) $^ -o $@ -lcppunit

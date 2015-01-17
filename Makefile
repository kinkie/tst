CXX=/usr/bin/g++
INCLUDES=-I/opt/local/include
CFLAGS = -g $(INCLUDES)
CXXFLAGS = -g -std=c++11 $(INCLUDES)
LDFLAGS=-L/opt/local/lib
TESTS = TestCompactArrayTrieNode testCompactTrie
#LIBS = libTernaryTrie.a

all: $(LIBS) check

check: $(TESTS)
	for a in $^; do ./$$a; done

clean:
	-rm $(LIBS) $(TESTS) *.o

#libTernaryTrie.a: TernaryTrie.o
#	ar cru $@ $^
#	

TestCompactArrayTrieNode.o: CompactArrayTrieNode.h TestCompactArrayTrieNode.cc TestCompactArrayTrieNode.h

testCompactTrie.o: testCompactTrie.cc testCompactTrie.h CompactTrie.h CompactArrayTrieNode.h

TestCompactArrayTrieNode: TestCompactArrayTrieNode.o
	g++ $(CXXFLAGS) $(LDFLAGS) $< -o $@ -lcppunit

testCompactTrie: testCompactTrie.o CompactTrie.o
	g++ $(CXXFLAGS) $(LDFLAGS) $< -o $@ -lcppunit
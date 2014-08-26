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

TestCompactArrayTrieNode.o: CompactArrayTrieNode.h TestCompactArrayTrieNode.cc TestCompactArrayTrieNode.h

TestCompactArrayTrieNode: TestCompactArrayTrieNode.o
	g++ $(CXXFLAGS) $< -o $@ -lcppunit

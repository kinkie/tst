CFLAGS = -g
CXXFLAGS = -g

all: libTernaryTrie.a TestTernaryTrie

libTernaryTrie.a: TernaryTrie.o TernaryTrie.h
	ar rcs $@ $^
	
TestTernaryTrie: TestTernaryTrie.cc libTernaryTrie.a
	g++ -g TestTernaryTrie.cc -o $@ -L. -lTernaryTrie -lcppunit
	
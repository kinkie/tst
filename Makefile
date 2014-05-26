all: libTernaryTrie.a testTernaryTrie

libTernaryTrie.a: TernaryTrie.cc TernaryTrie.h
	ar cru $@ $^
	
testTernaryTrie: testTernaryTrie.cc libTernaryTrie.a 
	gcc -g testTernaryTrie.cc -o $< -L. -lTernaryTrie
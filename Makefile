CFLAGS = -g
CXXFLAGS = -g -std=c++11

all: libTernaryTrie.a TestTernaryTrie
	./TestTernaryTrie

clean:
	rm *.a *.o

libTernaryTrie.a: TernaryTrie.o
	ar cru $@ $^
	
TestTernaryTrie: TestTernaryTrie.o libTernaryTrie.a TernaryTrie.h
	g++ $(CXXFLAGS) $^ -o $@ -L. -lTernaryTrie -lcppunit

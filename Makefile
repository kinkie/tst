CXX=/usr/bin/g++
CFLAGS = -g
CXXFLAGS = -g -std=c++11
TESTS = TestTernaryTrie
LIBS = libTernaryTrie.a

all: $(LIBS) check

check: $(TESTS)
	for a in $^; do ./$$a; done

clean:
	-rm $(LIBS) $(TESTS) *.o

libTernaryTrie.a: TernaryTrie.o
	ar cru $@ $^
	
TestTernaryTrie: TestTernaryTrie.o TernaryTrie.h
	g++ $(CXXFLAGS) $^ -o $@ -L. -lTernaryTrie -lcppunit

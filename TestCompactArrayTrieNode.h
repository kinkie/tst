#ifndef SQUID_TESTTERNARYTRIE_H_
#define SQUID_TESTTERNARYTRIE_H_

#include <cppunit/extensions/HelperMacros.h>

/**
 *
 */
class TestCompactArrayTrieNode  : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TestCompactArrayTrieNode );
//    CPPUNIT_TEST(  );
    CPPUNIT_TEST( addToNode );
    CPPUNIT_TEST( findInNode );
    CPPUNIT_TEST( nodeIterator );

    CPPUNIT_TEST_SUITE_END();

protected:
//  void testWhatever();
    void addToNode();
    void findInNode();
    void nodeIterator();
};

#endif /* SQUID_TESTTERNARYTRIE_H_ */

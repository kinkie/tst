#ifndef SQUID_TESTTERNARYTRIE_H_
#define SQUID_TESTTERNARYTRIE_H_

#include <cppunit/extensions/HelperMacros.h>

/**
 *
 */
class TestTernaryTrie  : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TestTernaryTrie );
    CPPUNIT_TEST( testClearAndSize );
    CPPUNIT_TEST( testCount );
    CPPUNIT_TEST( testErase );
    CPPUNIT_TEST( testInsert );
    CPPUNIT_TEST( testOperatorSquareBrackets );
    CPPUNIT_TEST_SUITE_END();

protected:
    void testClearAndSize();
    void testCount();
    void testErase();
    void testInsert();
    void testOperatorSquareBrackets();
};

#endif /* SQUID_TESTTERNARYTRIE_H_ */

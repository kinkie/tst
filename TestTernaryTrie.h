#ifndef SQUID_TESTTERNARYTRIE_H_
#define SQUID_TESTTERNARYTRIE_H_

#include <cppunit/extensions/HelperMacros.h>

/**
 *
 */
class TestTernaryTrie  : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TestTernaryTrie );
    CPPUNIT_TEST( testInsert );
    CPPUNIT_TEST( testErase );
    CPPUNIT_TEST( testOperatorSquareBrackets );
    CPPUNIT_TEST( testCount );
    CPPUNIT_TEST_SUITE_END();

protected:
    void testInsert();
    void testErase();
    void testCount();
    void testOperatorSquareBrackets();
};

#endif /* SQUID_TESTTERNARYTRIE_H_ */

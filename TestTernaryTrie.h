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
    CPPUNIT_TEST_SUITE_END();

protected:
    void testInsert();
    void testErase();
};

#endif /* SQUID_TESTTERNARYTRIE_H_ */

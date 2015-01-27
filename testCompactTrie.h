#ifndef SQUID_TESTCOMPACTTRIE_H_
#define SQUID_TESTCOMPACTTRIE_H_

#include "CompactTrie.h"

#include <cppunit/extensions/HelperMacros.h>

/**
 *
 */
class TestCompactTrie  : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TestCompactTrie );
    CPPUNIT_TEST( testInsert );
    CPPUNIT_TEST( testFind );
    CPPUNIT_TEST( testIterator );
    CPPUNIT_TEST( testEmpty );
    CPPUNIT_TEST( testContents );
    //    CPPUNIT_TEST(  );
    CPPUNIT_TEST_SUITE_END();

protected:
    void testInsert();
    void testFind();
    void testIterator();
    void testEmpty();
    void testContents();
    //  void testWhatever();
};



#endif /* SQUID_TESTCOMPACTTRIE_H_ */

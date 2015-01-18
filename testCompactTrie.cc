#include "testCompactTrie.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

typedef CompactTrie<std::string, int> CT;

void
TestCompactTrie::testInsert()
{
    CT ct;
    ct.insert("foo",1);
    ct.insert("bar",2);
    CPPUNIT_ASSERT(ct.has("foo"));
    CPPUNIT_ASSERT(ct.has("bar"));
    CPPUNIT_ASSERT(!ct.has("gazonk"));
}

void
TestCompactTrie::testFind()
{
    CT ct;
    ct.insert("foo",1);
    ct.insert("bar",2);
    ct.insert("foo.",3);
    {
        CT::value_type rv = ct.find("foo");
        CPPUNIT_ASSERT(rv != ct.end()); // found
        CPPUNIT_ASSERT_EQUAL(rv.second,1); // mapped data match
        CPPUNIT_ASSERT(ct.find("gazonk") == ct.end()); // not found
    }

    {
        // check CompactTrie::end() uniqueness
        CT::value_type test_end = std::make_pair(std::string(), int());
        CPPUNIT_ASSERT(ct.find("gazonk") != test_end);
    }

    {
        // prefix find tests
        CT::value_type rv;

        rv = ct.prefixFind("foo"); // exact match
        CPPUNIT_ASSERT(rv != ct.end());

        rv = ct.prefixFind("fooo"); //prefix match
        CPPUNIT_ASSERT(rv != ct.end());

        rv = ct.prefixFind("go"); //prefix match, not found
        CPPUNIT_ASSERT(rv == ct.end());
    }
}


/*** boilerplate starts here ***/

CPPUNIT_TEST_SUITE_REGISTRATION( TestCompactTrie );

int
main (int argc, char ** argv)
{
    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );

    // Add a listener that print dots as test run.
    // use BriefTestProgressListener to get names of each test
    // even when they pass.
    CPPUNIT_NS::TextTestProgressListener progress;
    controller.addListener( &progress );

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( controller );

    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
    outputter.write();

    return result.wasSuccessful() ? 0 : 1;
}



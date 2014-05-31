#include "TernaryTrie.h"
#include "TestTernaryTrie.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** tests start here **/

typedef TernaryTrie<std::string,int> TT;
void
TestTernaryTrie::testInsert()
{
    // single-argument insert
    {
        TT tt;
        tt.insert(std::make_pair(std::string("foo"), 1));
        tt.insert(std::make_pair(std::string("bar"), 2));
        CPPUNIT_ASSERT_EQUAL(1, tt.at("foo"));
        CPPUNIT_ASSERT_EQUAL(2, tt.at("bar"));
        CPPUNIT_ASSERT_THROW(tt.at("gazonk"),std::out_of_range);
    }
    // range insert
    {
        TT tt;
        typedef std::vector<TT::value_type> V;
        V v;
        v.push_back(std::make_pair("foo",1));
        v.push_back(std::make_pair("bar",2));
        tt.insert(v.begin(),v.end());
        CPPUNIT_ASSERT_EQUAL(1, tt.at("foo"));
        CPPUNIT_ASSERT_EQUAL(2, tt.at("bar"));
        CPPUNIT_ASSERT_THROW(tt.at("gazonk"),std::out_of_range);
    }
}

void
TestTernaryTrie::testErase()
{
    TT tt;
    tt.insert(std::make_pair(std::string("foo"), 1));
    tt.insert(std::make_pair(std::string("bar"), 2));
    CPPUNIT_ASSERT_EQUAL(1, tt.at("foo"));
    CPPUNIT_ASSERT_EQUAL(2, tt.at("bar"));
    tt.erase(std::string("foo"));
    CPPUNIT_ASSERT_THROW(tt.at("foo"),std::out_of_range);
}


/*** boilerplate starts here ***/

CPPUNIT_TEST_SUITE_REGISTRATION( TestTernaryTrie );

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


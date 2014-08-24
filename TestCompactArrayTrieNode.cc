#include "TestCompactArrayTrieNode.h"
#include "CompactArrayTrieNode.h"

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

void
TestCompactArrayTrieNode::addToNode()
{
    CompactArrayTrieNode<std::string,int> tn;
    CPPUNIT_ASSERT_EQUAL(true, tn.add("foo",1));
    CPPUNIT_ASSERT_EQUAL(true, tn.add("bar",2));
    int i=3;
    CPPUNIT_ASSERT_EQUAL(true, tn.add("gazonk",i)); // non-const value
    CPPUNIT_ASSERT_EQUAL(true, tn.add("foo",4)); // overwrite
}

void
TestCompactArrayTrieNode::findInNode()
{
    CompactArrayTrieNode<std::string,int> tn;
    tn.add("foo",1);
    tn.add("bar",2);
    tn.find("foo") != nullptr;
}

/*** boilerplate starts here ***/

CPPUNIT_TEST_SUITE_REGISTRATION( TestCompactArrayTrieNode );

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


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
    ct.insert("baz",3);
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
    ct.insert("foo.",3); //terminator WITH nonterminated variant
    ct.insert("baz.", 4);  //terminator, no nonterminated variant
    {
        CT::iterator rv = ct.find("foo");
        CPPUNIT_ASSERT(rv != ct.end()); // found
        CPPUNIT_ASSERT_EQUAL(rv->second,1); // mapped data match
        CPPUNIT_ASSERT(ct.find("gazonk") == ct.end()); // not found
    }

    {
        // prefix find tests
        CT::iterator rv;

        rv = ct.prefixFind("foo"); // exact match
        CPPUNIT_ASSERT(rv != ct.end());

        rv = ct.prefixFind("fooo"); //prefix match
        CPPUNIT_ASSERT(rv != ct.end());

        rv = ct.prefixFind("go"); //prefix match, not found
        CPPUNIT_ASSERT(rv == ct.end());
    }

    {
        CT::iterator rv;

        rv = ct.prefixFind("foo.", '.'); // exact match
        CPPUNIT_ASSERT( rv != ct.end());

        rv = ct.prefixFind("foo", '.'); // exact match, termination is irrelevant
        CPPUNIT_ASSERT( rv != ct.end());

        rv = ct.prefixFind("foooo", '.'); // prefix match but no terminator
        CPPUNIT_ASSERT( rv == ct.end());

        rv = ct.prefixFind("foo.bar", '.'); // prefix match and terminator
        CPPUNIT_ASSERT( rv != ct.end());

        rv = ct.prefixFind("baz.bar", '.'); // prefix match and terminator
        CPPUNIT_ASSERT( rv != ct.end());

        rv = ct.prefixFind("baz", '.');
        CPPUNIT_ASSERT( rv != ct.end());

        rv = ct.prefixFind("baz.www", '.');
        CPPUNIT_ASSERT( rv != ct.end());

        rv = ct.prefixFind("bazz.www", '.');
        CPPUNIT_ASSERT( rv == ct.end());

    }
}

void
TestCompactTrie::testIterator()
{
    CT ct;

    ct.insert("foo",1);
    ct.insert("bar",2);
    ct.insert("gazonk",3);

    {
        CT::iterator i1=ct.find("foo");
        CT::iterator i2=ct.find("foo");
        CPPUNIT_ASSERT(i1 == i2);

        i2=ct.find("bar");
        CPPUNIT_ASSERT(i1 != i2);

        i2=ct.find("not found");
        CPPUNIT_ASSERT(i1 != i2);
    }

    {
        // note: this behavior DIFFERS from std:: iterators.
        // the end() iterator is SHARED between all class intances
        CT ct2;
        CT::iterator e1=ct.find("not found");
        CT::iterator e2=ct2.find("not found");
        CPPUNIT_ASSERT(e1 == e2);
    }
}

void
TestCompactTrie::testEmpty()
{
    CT ct;
    CPPUNIT_ASSERT(ct.empty());
    ct.insert("foo",1);
    CPPUNIT_ASSERT(!ct.empty());
}

void
TestCompactTrie::testContents()
{
    CT ct;
    CPPUNIT_ASSERT(ct.contents().empty());
    ct.insert("foo",1);
    CPPUNIT_ASSERT(!ct.contents().empty());
    ct.insert("foo1",1);
    ct.insert("foo0",1);
    CPPUNIT_ASSERT(ct.contents().size() == 3);
    CPPUNIT_ASSERT(ct.contents()[0]->first == "foo");
    CPPUNIT_ASSERT(ct.contents()[1]->first == "foo0");
    CPPUNIT_ASSERT(ct.contents()[2]->first == "foo1");
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



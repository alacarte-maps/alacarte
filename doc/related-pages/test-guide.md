The Test Guide
=============
As you might already know, we use `boost::test`. This serves as an entry point to our current organization.
For more detailed information, see the [documentation of the boost testing framework][].


All tests are in the `tests` folder, which might come as a suprise to you.
After compiling, there should be 4 test programs:

1. general
2. utils
3. importer
4. server

In this folder, there's always a `…_test_module.cpp`. It only describes the test and does not need to be changed.
Then you can add a subfolder for each class to test and place the tests in there.
For example, if you want to add a test for the `Geodata` class, you do that in
`tests/general/geodata-test/geodata_functional_test.cpp`.


### boost::unit_test ###
Ok, now to the test itself. Boost uses a suite tree to organize tests.
The top level is the master suite. If you don't specify another suite, your tests will be registered here.
With `BOOST_AUTO_TEST_SUITE`, you can open your own suite (internally, it's just another namespace).
Then use `BOOST_AUTO_TEST_CASE` to start and stop your test.
Here's a simple Point test:

~~~~~~~~~~~~~~~~~{.cpp}
#include <boost/test/unit_test.hpp>

#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_point_test)

BOOST_AUTO_TEST_CASE( point_test )
{
	BOOST_CHECK_EQUAL( Point(3,3), Point(3,3));
	BOOST_CHECK_EQUAL( Point(4,3), Point(3,3));
	BOOST_CHECK_EQUAL( Point(5,3), Point(3,3));
}

// close the suite in the end
BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)

~~~~~~~~~~~~~~~~~

Output:

~~~~~~~~~~~~
Running 1 test case...
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(10): error in "point_test": check Point(4,3) == Point(3,3) failed [4, 3 != 3, 3]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(11): error in "point_test": check Point(5,3) == Point(3,3) failed [5, 3 != 3, 3]

*** 2 failures detected in test suite "utils test"
~~~~~~~~~~~~

### Test Cases with Parameters ###

If you want to test your own test function with different parameters, we built a little macro to help with that:

~~~~~~~~~~~~~~~~~~{.cpp}
#include "../../tests.hpp"
#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_point_test)


void equal_test(const Point& p1, const Point& p2)
{
	BOOST_CHECK_EQUAL( p1, p2);
}

ALAC_PARAM_TEST_CASE(equal_test, Point(3, 4), Point(3, 4));
ALAC_PARAM_TEST_CASE(equal_test, Point(3, 8), Point(3, 4));
ALAC_PARAM_TEST_CASE(equal_test, Point(3, 4), Point(3, 8));


BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
~~~~~~~~~~~~~~~~~~

Output:

~~~~~~~~~~~~~~~~~~
Running 3 test cases...
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(9): error in "equal_test_in_13": check p1 == p2 failed [3, 8 != 3, 4]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(9): error in "equal_test_in_14": check p1 == p2 failed [3, 4 != 3, 8]

*** 2 failures detected in test suite "utils test"
~~~~~~~~~~~~~~~~~~

### Test Cases with Context ###

Sometimes you want to prepare a test, then execute the test and (sometimes 😉) clean up.

~~~~~~~~~~~~~~~~~~~{.cpp}
#include "../../tests.hpp"
#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_point_test)

struct simple_check_test
{
	// We prepare our test in the contstructor.
	// In doing so, we can add an arbitrary number of parameters.
	simple_check_test(const Point& p)
	{
		point = p;
	}

	~simple_check_test()
	{
		// Here we would clean up, but there's nothing to do.
	}

	// Here we write our test case itself.
	// They can be named arbitrarily and can take an arbitrary number of parameters.
	void compare(const Point& p)
	{
		BOOST_CHECK_EQUAL(point, p);
	}

	// We can create members for our test, too.
	Point point;
};

// Here we create our test.
// We specify the above structure
// and afterwards, there are the arguments for the constructor
ALAC_START_FIXTURE_TEST(simple_check_test, Point(5, 5))
	// All following tests are executed on the above struct.
	// Please note: The point attribute is now a (5,5), of course.
	// Now we specify the test cases and call them with parameters.
	ALAC_FIXTURE_TEST(compare, Point(4,5));
	ALAC_FIXTURE_TEST(compare, Point(4,6));
	// Usually, the line is added to the test name to make it unique
	// Most macros can be called suffixed with _NAMED, though, which allows to add your own name (here compare_4_and_7)
	// (see output for difference)
	ALAC_FIXTURE_TEST_NAMED(compare, compare_4_and_7, Point(4,7));
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
~~~~~~~~~~~~~~~~~~~
Output:

~~~~~~~~~~~~~~~~~~~
Running 3 test cases...
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(27): error in "compare_in_34": check point == p failed [5, 5 != 4, 5]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(27): error in "compare_in_35": check point == p failed [5, 5 != 4, 6]
/home/tobi/c++/alacarte/tests/utils/point-test/simple_point_test.cpp(27): error in "compare_4_and_7": check point == p failed [5, 5 != 4, 7]

*** 3 failures detected in test suite "utils test"
~~~~~~~~~~~~~~~~~~~

[documentation of the boost testing framework]: http://www.boost.org/doc/libs/1_55_0/libs/test/doc/html/index.html

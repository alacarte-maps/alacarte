The Macro Guide
===============

Here is a list of C++ preprocessor macros and definitions used by alaCarte.

When I say definitions, I mean some compile-time options in macro-format.

Definitions
-----------

With definitions you can control alacartes behavior at compile time.


### ALACARTE_NO_BACKBONE ###

Use this definition (in compiler settings) to prevent alacarte from catching every
exception at the basement (see \ref Application).


Macros
------

With macros, it “should” (😉) be easyer to program. Use it in your code and it will be replaced with something awesome!!!

### TESTING ###

If `ALACARTE_TEST` is defined, `TESTING` will be replaced with `virtual`. In the other case it will disappear.

~~~~~~~~~~~~~{.cpp}
#ifdef ALACARTE_TEST
#	define TESTABLE virtual
#else
#	define TESTABLE
#endif
~~~~~~~~~~~~~



### NOT_IMPLEMENTED ####

If this macro is reached, an NotImplementedException is thrown.

~~~~~~~~~~~~~~~~~~{.cpp}
#define NOT_IMPLEMENTED() {BOOST_THROW_EXCEPTION(excp::FileNotFoundException());}
~~~~~~~~~~~~~~~~~~


### IMPLEMENTATION_TODO(_text) ###

Use this to indicate, that something is not implemented yet.
This will produce a log message at `std::cout` with the given `_text` ONCE!


### ALAC_PARAM_TEST_CASE(_name, ...) ###

Creates a new test case with parameters.

`name` is the name of the testcase.

`...` are the typed parameters for the test case.

Example:
~~~~~~~~~~~~{.cpp}
int Add(int x, int y) { return x + y; }

ALAC_PARAM_TEST_CASE(AddTest, int x, int y, int expected)
{
	BOOST_CHECK_EQUAL(Add(x, y), expected);
}
~~~~~~~~~~~~
Note that `ALAC_PARAM_TEST_CASE` simply defines a function signature!
The above code is equavalent with the following:

~~~~~~~~~~~~{.cpp}
void AddTest(int x, int y, int expected)
{
	BOOST_CHECK_EQUAL(Add(x, y), expected);
}
~~~~~~~~~~~~




### ALAC_PARAM_TEST_NAMED(_func, _name, ...) ###

Calls a test case with given parameters (actual).

`_func` is the name of the test case (e.g. _AddTest_)

`_name` is the name of the test. Will be printed on the output if the test case fails with the given parameter.

[...] are the actual parameters passed to the test case.

Example:
~~~~~~~~~~~~~{.cpp}
ALAC_PARA_TEST_NAMED(AddTest, Tests_If_AddTest_Is_8_with_3_and_5, 3, 5, 8);
~~~~~~~~~~~~~




### ALAC_PARAM_TEST(_func, ...) ###

Calls a test case with given parameters (actual). A name for the test will be created depending on the line this macro
is used.  If you use this macro for example in line 78 and the test is `AddTest`, then the test is named
`AddTest_in_78`.

This macro simply wraps `ALAC_PARAM_TEST_NAMED`. See there for more information about parameters.

~~~~~~~~~~~~~~{.cpp}
ALAC_PARAN_TEST_(AddTest, 3, 5, 8);
~~~~~~~~~~~~~~



### ALAC_START_FIXTURE_TEST_NAMED(_name, _fixture, ...)	###

Starts a named fixture test. Creates `_fixture` with the given parameters `...`.
Then tests can be applied. To end the test use `ALAC_END_FIXTURE_TEST`.

See guide for more information and examples.


### ALAC_START_FIXTURE_TEST(_test, ...) ###

Same as `ALAC_START_FIXTURE_TEST_NAMED` but with auto generated name.


### ALAC_FIXTURE_TEST_NAMED(_func, _name, ...) ###

Calls a test case with actual parameters on a fixture. The test can be named.

See guide for more informations and examples.


### ALAC_FIXTURE_TEST(_func, ...) ###

Same as `ALAC_FIXTURE_TEST_NAMED` but with auto generated name.



### ALAC_END_FIXTURE_TEST() ###

Marks the end of an fixture test started with `ALAC_START_FIXTURE_TEST` or `ALAC_START_FIXTURE_TEST_NAMED`.

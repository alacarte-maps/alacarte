The Exception Guide
==========

One could throw almost anything in C++, but as we want to have a consistent programming style, let's use
`boost::exceptions` for everything.
But it's important to understand them first.

So, for the start, please read the [Motivation for boost exceptions][] first!

Finished? Now that you're back here, let's continue.
Our concept is to stack all exceptions in `utils/exceptions.hpp`, in the `excp` namespace.

* If you want to send information with exceptions, look in the file how that's done. 😉
* If you want to define a new exception, use the already existing ones as template.

[Motivation for boost exceptions]: http://www.boost.org/doc/libs/1_55_0/libs/exception/doc/motivation.html

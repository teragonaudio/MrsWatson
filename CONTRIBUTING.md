Contributing to MrsWatson
=========================

If you would like to help improve MrsWatson, there are several ways which
you can help out. 


Non-Programming Contributions
-----------------------------

One of the biggest ways to contribute to MrsWatson is to test it. One of the
project goals is to run a wide variety of plug-ins on different platforms.
More testing is needed, especially on Windows, where a very large number of
freeware and shareware plug-ins are to be found. If you encounter any
problems, run `mrswatson --error-report` (along with the other options which
generated the error).  This will generate an error report to file that can
be sent to support in order to fix the bugs. 


Programming Contributions
-------------------------

MrsWatson is open source and hosted on GitHub, pull requests are gladly
accepted, assuming that they follow the coding conventions and do not
introduce any new bugs in the code.


Coding Style Conventions
------------------------

MrsWatson is written in C, although there is some C++ code used to
communicate with VST plug-ins. In order to make the code easy to understand,
and portable to a wide number of platforms, C remains the preferred language
of choice. That said, the style of C used in MrsWatson takes some influence
from Python and Java. It may seem a bit unusual at first, but there are
really only a few simple conventions in the project. This convention was
designed with the following goals:

* To make highly readable object-oriented code, without needing to use C++.
  Although C++ is a nice language, it also has a large feature set which
  makes it difficult to maintain a consistent coding style as a project
  grows, and as it incorporates more third-party libraries.
* To make memory management very easy and avoid leaking. MrsWatson is
  rigorously tested with valgrind before each release in order to ensure
  that there is no memory corruption or leaked memory in the application.
* To keep build times fast.


Classes
-------

Classes in MrsWatson are simple structs with some associated functions that
are able to operate on them. A naming convention is used to associate the
class with the associated functions. Each of these functions takes in a
"self" argument as its first parameter, just like in Python. Also like
Python, there are no private members in the structs, so a leading underscore
is used to indicate that a member should not be directly accessed.

Instead of passing raw pointers throughout the source code, typedefs are
used to make the code easier to understand. Thus, a sample class might look
like this:

    typedef struct {
      int foo;
      char _bar;
      char* baz;
    } MyClassMembers;
    typedef MyClassMembers* MyClass;

Each class should provide a constructor function and a destructor function
so that the caller does not need to manually allocate and free the
associated memory. By convention, these functions should be prefixed with
"new" and "free". Continuing our example:

    MyClass newMyClass(void) {
      MyClass result = (MyClass)malloc(sizeof(MyClassMembers));
      result->foo = 0;
      result->_bar = 'a';
      result->baz = NULL;
      return result;
    }

    void freeMyClass(MyClass self) {
      if(self != NULL) {
        free(self->baz);
        free(self);
      }
    }

Any member functions for this class should begin with the class name. This
begins to get a little verbose, but at least it makes the code easy to
understand. Continuing our example:

    char myClassGetBar(MyClass self) {
      return self->_bar;
    }

    size_t myClassBazLength(MyClass self) {
      return self->baz != NULL ? strlen(self->baz) : 0;
    }

The result is a convention that allows one to write code like this:

    MyClass whatever = newMyClass();
    if(myClassBazLength(whatever)) {
      char ch = myClassGetBar(whatever);
    }
    freeMyClass(whatever);


Internal API
------------

Within the "base" package, there are a number of useful utility classes
which should be used rather than reinventing the wheel. Namely, classes for
handling files, strings, and linked lists are found here. Please refer to
the test suite in order to get a feel for usage of these classes.


Indenting
---------

The project code is invented with 2 spaces (no tabs). Please refer to the
existing project code to get a feel for the conventions used with spaces,
brackets, braces, etc.


Testing
-------

If you plan on adding new features to MrsWatson, please also add tests for
them. The mrswatsontest project shows how to add new tests for internal
functions, see the documentation in the "Building" file for how to build and
run the test suite.


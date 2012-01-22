Binary Installation
===================

MrsWatson is provided in binary form for Windows and MacOSX. You can find a
statically-compiled binary for these platforms from the [official project
webpage](http://github.com/teragonaudio/MrsWatson).

To install these, simply copy the `mrswatson` file to somewhere in
your PATH. Make sure that the file has executable rights on unix-based
platforms (chmod +x).

Building MrsWatson from Source
==============================

To build MrsWatson on any platform, you will need to acquire a copy of the VST
SDK source code. Due to licensing restrictions of the VST SDK, this code is
not distributed with MrsWatson itself. It can be downloaded from [Steinberg's
Developer Page](http://www.steinberg.net/en/company/3rd_party_developer.html),
and then extracted to the `vendor/vstsdk2.4` subdirectory of the project
before compiling.

Mac OSX
-------

To build MrsWatson on Mac OSX, you will need to have Apple's Developer Tools
installed. An Xcode project is provided which will automatically build the
project and copy the resulting binary to `/usr/local/bin`.

Make sure that your user has permissions to write the the `/usr/local/bin`
directory. Otherwise, you will need to go through the tedious step of
building the product for archiving it and copying the resulting file to a
useful location.

If you don't want to deal with Xcode, you can go the unix route and install
using:

    ./configure
    make
    make install

Note that you will still need Xcode installed for this to work, as it provides
gcc and other necessary build tools.

Windows
-------

Because Visual Studio does not support C99, MrsWatson is built on this
platform using [MinGW MSYS](http://www.mingw.org). Download and install MSYS,
including extra development tools and such. You will then need to [add the
MSYS directories to the path](http://www.mingw.org/wiki/Getting_Started#toc4).

Although the documentation implies that `C:\MinGW\MSYS\1.0\bin` is an optional
path, it is needed for several GNU tools.

After you have installed MSYS, you can unpack the MrsWatson source, and build
it with:

    ./configure
    make

This will create `source\mrswatson.exe`, which can then be copied to the
location of your choice.

Linux
-----

**NOTE: Linux is currently an experimental platform!**

MrsWatson may not work for you without some extra work, but linux is a desired
target platform, so please submit any necessary patches or bug reports to the
official project webpage. As MrsWatson is a 32-bit program, you will need to
have a complete 32-bit build environment set up for your machine.

On Ubuntu, this can be done by installing the following packages:

  * libstdc++
  * g++-multilib
  * ia32-libs
  * libaudiofile0
  * libaudiofile-dev

This is in addition to the standard build tools (autoconf, gcc, g++, etc).
Otherwise, building on linux should be a matter of:

    ./configure
    make
    make install


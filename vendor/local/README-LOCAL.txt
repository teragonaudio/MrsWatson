This directory contains the built third-party libraries needed by MrsWatson. The
actual build process should be executed by CMake in the root directory, and the
resulting directory structure here should resemble /usr/local.

As MrsWatson requires 32-bit builts of several libraries, they must be compiled
and statically linked to ease installation on 64-bit systems.

You should not need to manually install or otherwise alter this directory, and
it should be safe to remove. However, "make clean" should be run to force
rebuilding the contents of local in addition to removing any subdirectories
here.


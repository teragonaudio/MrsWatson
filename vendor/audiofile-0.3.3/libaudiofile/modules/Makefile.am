noinst_LTLIBRARIES = libmodules.la

INCLUDES = -I$(srcdir)/..

libmodules_la_SOURCES = \
	PCM.cpp PCM.h \
	MSADPCM.cpp MSADPCM.h \
	IMA.cpp IMA.h adpcm.c adpcm.h \
	G711.cpp G711.h \
	ModuleState.cpp ModuleState.h \
	Module.cpp Module.h \
	SimpleModule.cpp SimpleModule.h \
	RebufferModule.cpp RebufferModule.h \
	FileModule.cpp FileModule.h

# GNU gcc
# AM_CFLAGS = -Wall -g
# SGI MIPSpro cc
# AM_CFLAGS = -fullwarn -g
# No debugging.
AM_CFLAGS = -DNDEBUG -Wall $(WERROR_CFLAGS) $(COVERAGE_CFLAGS)
AM_CXXFLAGS = $(AM_CFLAGS) -fno-rtti -fno-exceptions

## Process this file with automake to produce Makefile.in

SRC_SUBDIRS = libaudiofile sfcommands test examples
SUBDIRS = $(SRC_SUBDIRS) docs

EXTRA_DIST = \
	ACKNOWLEDGEMENTS \
	NOTES \
	README \
	TODO \
	COPYING.GPL \
	configure configure.ac \
	audiofile.spec.in \
	audiofile.pc.in \
	audiofile-uninstalled.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audiofile.pc

dist-hook: audiofile.spec
	cp audiofile.spec $(distdir)

if ENABLE_COVERAGE
coverage:
	$(MAKE) -C test coverage-reset
	$(MAKE) -C test check
	$(MAKE) -C test coverage-report
else
coverage:
	@echo "Code coverage is not enabled."
	@exit 1
endif
.PHONY: coverage

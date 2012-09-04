AM_CPPFLAGS = -I../source -I../vendor/vstsdk2.4/pluginterfaces/vst2.x -I../vendor/audiofile-0.3.3/libaudiofile $(HOST_CPPFLAGS)
AM_LDFLAGS = $(HOST_LDFLAGS)

bin_PROGRAMS = mrswatson
mrswatson_SOURCES = main.c
mrswatson_LDADD = $(HOST_LIBS) ../source/libmrswatson.a
SUBDIRS=../source

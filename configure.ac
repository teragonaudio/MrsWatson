# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.61])
AC_INIT([MrsWatson], [0.9.1], [support@teragonaudio.com])
AM_INIT_AUTOMAKE([MrsWatson], [0.9.1])

AC_CANONICAL_HOST
case "${host}" in
  *-*-darwin*)
    HOST_CPPFLAGS='-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -std=c99 -O3 -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -DMACOSX=1 -fasm-blocks -mmacosx-version-min=10.4'
    HOST_LDFLAGS='-arch i386 -framework Carbon -framework CoreFoundation'
    ;;
  *-*-mingw*)
    HOST_CPPFLAGS='-O3 -std=c99 -DWINDOWS=1'
    ;;
  *-*-linux*)
    HOST_CPPFLAGS='-fmessage-length=0 -m32 -pipe -Wno-trigraphs -std=c99 -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -DLINUX=1 -D__cdecl=""'
    HOST_LDFLAGS='-m32'
    ;;
  *)
    AC_MSG_RESULT([Unsupported operating system: ${host}])
    ;;
esac
AC_SUBST([HOST_CPPFLAGS])
AC_SUBST([HOST_LDFLAGS])
AC_SUBST([HOST_LIBS])

AC_CONFIG_SRCDIR([source])
# AC_CONFIG_HEADERS([config.h])
# AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
# AC_PATH_X
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h audiofile.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor gethrtime gettimeofday memset pow strcasecmp strchr strdup strncasecmp strrchr strstr strtol])
AC_CHECK_LIB([dl], [dlopen])

# Optional libraries
AC_ARG_WITH([audiofile],
            [AS_HELP_STRING([--without-audiofile],
                            [use audiofile for AIFF & WAVE files])],
            [],
            [with_audiofile=check])
LIBAUDIOFILE=
AS_IF([test "x$with_audiofile" != xno],
      [AC_CHECK_LIB([audiofile], [afReadFrames, afWriteFrames],
                    [AC_SUBST([LIBAUDIOFILE], ["-laudiofile"])
                    AC_DEFINE([HAVE_LIBAUDIOFILE], [1],
                              [Define if you have libaudiofile])
                    ],
                    [])])

AC_ARG_WITH([flac],
            [AS_HELP_STRING([--with-flac],
                            [add support for FLAC files])],
            [],
            [with_flac=no])
LIBFLAC=
AS_IF([test "x$with_flac" != xno],
      [AC_CHECK_LIB([flac], [afReadFrames, afWriteFrames],
                    [AC_SUBST([LIBFLAC], ["-lflac"])
                    AC_DEFINE([HAVE_LIBFLAC], [1],
                              [Define if you have libflac])
                    ],
                    [])])

AC_ARG_WITH([lame],
            [AS_HELP_STRING([--with-lame],
                            [add support for MP3 files])],
            [],
            [with_lame=no])
LIBLAME=
AS_IF([test "x$with_lame" != xno],
      [AC_CHECK_LIB([lame], [afReadFrames, afWriteFrames],
                    [AC_SUBST([LIBLAME], ["-llame"])
                    AC_DEFINE([HAVE_LIBLAME], [1],
                              [Define if you have liblame])
                    ],
                    [])])

AC_ARG_WITH([vorbis],
            [AS_HELP_STRING([--with-vorbis],
                            [add support for Ogg files])],
            [],
            [with_vorbis=no])
LIBVORBIS=
AS_IF([test "x$with_vorbis" != xno],
      [AC_CHECK_LIB([vorbis], [afReadFrames, afWriteFrames],
                    [AC_SUBST([LIBVORBIS], ["-lvorbis"])
                    AC_DEFINE([HAVE_LIBVORBIS], [1],
                              [Define if you have libvorbis])
                    ],
                    [])])


AC_CONFIG_HEADERS(config.h)
AC_OUTPUT(Makefile source/Makefile)

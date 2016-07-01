/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

// Make sure that Windows doesn't redefine these symbols before we include
// the Windows.h header.
#define NOMINMAX

// This symbol is defined somewhere in deep the Win32 API, but audiofile
// also defines it in an enum. So we must define it before including
// Windows.h, but then undef it before compiling anything.
#define WAVE_FORMAT_PCM
#include <Windows.h>
#include <io.h>
#undef WAVE_FORMAT_PCM

#if USE_FLAC
// This flag is also defined for the config.h file belonging to FLAC. It
// needs to be set it here as well or else the symbol visibility will differ
// and the linker will complain about missing symbols from the FLAC library.
#define FLAC__NO_DLL 1

// Aside from the other problems with FLAC__ASSERT on Windows, it seems
// that MSVC compiler finds assert.h in FLAC before the system headers.
// So we undefine the macro and redefine it to nothing, which is perhaps
// a bit dangerous but necessary.
#define FLAC__ASSERT_H 1
#undef assert
#define assert(x)
#endif

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Whether FLAC is enabled. */
#define ENABLE_FLAC USE_FLAC

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 0

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #define HAVE_UNISTD_H 1 */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "audiofile"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "audiofile"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "audiofile 0.3.6"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "audiofile"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.3.6"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.3.6"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

// Types which windows does not define
#ifdef _WIN64
typedef unsigned __int64 ssize_t;
#else
typedef unsigned int ssize_t;
#endif

#define bzero(DATA, SIZE) memset(DATA, 0, SIZE)

// With MSVC 14 (VS 2015), Microsoft ships C99 compliant functions for snprintf and
// friends. So only define these macros for older versions of MSVC.
#if _MSC_VER < 1900
#define snprintf _snprintf
#define dup _dup
#define open _open
#endif

// Ignore VC compiler warnings
#pragma warning(disable: 4800)
#pragma warning(disable: 4244)
#pragma warning(disable: 4805)
#pragma warning(disable: 4996)
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4267)
#pragma warning(disable: 4244)
#pragma warning(disable: 4390)

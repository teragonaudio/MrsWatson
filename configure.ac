# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.61])
AC_INIT([MrsWatson], [1.0.0], [support@teragonaudio.com])
AM_INIT_AUTOMAKE([MrsWatson], [1.0.0])

AC_CONFIG_SRCDIR([source])
# AC_CONFIG_HEADERS([config.h])
# AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
# AC_PATH_X
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h])

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

AC_OUTPUT(Makefile source/Makefile)

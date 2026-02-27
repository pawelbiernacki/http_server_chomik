#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.72])
AC_INIT([http_chomik], [0.0.0], [pawel.f.biernacki@gmail.com])
AC_CONFIG_SRCDIR([inc/http_server_chomik.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([chomik], [chomik_open_file])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 inc/Makefile
                 src/Makefile])
AC_OUTPUT

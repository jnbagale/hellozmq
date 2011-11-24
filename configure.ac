AC_INIT(hellozmq, 0.2, jnbagale@gmail.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/])
AM_CONFIG_HEADER([src/config.h])

AC_DEFINE([PROGNAME],["hellozmq"],["hello zeromq"])
AC_DEFINE([DEFAULT_SERVER],["10.48.128.32"],["the default zeromq server"])
AC_DEFINE([DEFAULT_PORT],[8001],["the default zeromq broker port"])
AC_DEFINE([DEFAULT_GROUP],["world"],["the group everyone can join"])

# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC


# check for glib
PKG_CHECK_MODULES(GLIB, [glib-2.0])

#check for gthread
PKG_CHECK_MODULES(LIBZMQ, [libzmq] )

#check for gthread
PKG_CHECK_MODULES(GTHREAD, [gthread-2.0] )

# check for uuid
PKG_CHECK_MODULES(UUID, [uuid])

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT

AC_INIT(hellozmq, 0.2, jnbagale@gmail.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/])
AM_CONFIG_HEADER([src/config.h])

AC_DEFINE([PROGNAME],["hellozmq"],["hello zeromq"])
AC_DEFINE([DEFAULT_HOST],["127.0.0.1"],["the default zeromq server"])
AC_DEFINE([DEFAULT_SUB_PORT],[8100],["the default zeromq broker outbound port"])
AC_DEFINE([DEFAULT_PUB_PORT],[5556],["the default zeromq broker inbound port"])
AC_DEFINE([DEFAULT_GROUP],["world"],["the group everyone can join"])
AC_DEFINE([DEFAULT_TYPE],["both"],["the default client type to run"])

# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

# Check for pthreads library and header
AC_CHECK_LIB(pthread, pthread_create, , AC_MSG_ERROR(*** pthread library not found))
AC_CHECK_HEADERS(pthread.h,, [AC_MSG_ERROR([pthread.h required])])

#check for ZeroMQ
PKG_CHECK_MODULES(LIBZMQ, [libzmq] )

# check for uuid
PKG_CHECK_MODULES(UUID, [uuid])

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT

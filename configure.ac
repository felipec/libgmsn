AC_INIT([libmsn], [0.1.0], [felipe.contreras@gmail.com])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile lib/Makefile tests/Makefile])
AC_PROG_CC
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([GTHREAD], [gthread-2.0])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0])
PKG_CHECK_MODULES([OPENSSL], [openssl])
PKG_CHECK_MODULES([LIBXML], [libxml-2.0])

AC_OUTPUT

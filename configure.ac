dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(ionvif, 0.1)

dnl AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC

AC_USE_SYSTEM_EXTENSIONS


LT_INIT


PKG_CHECK_MODULES(IONVIF, [json-glib-1.0 glib-2.0 gobject-2.0 libipcam_base-0.1.0])

AC_ARG_ENABLE([openssl],
	AS_HELP_STRING([--enable-openssl],[Build with OpenSSL support (default: enabled)]),
	[enable_openssl=$enableval],
	[enable_openssl=yes])

if test "x$enable_openssl" != "xno"; then
	PKG_CHECK_MODULES(OPENSSL,
		[openssl],
		have_openssl=yes,
		have_openssl=no)
	if test "x$have_openssl" = "xyes"; then
		enable_openssl=yes
	else
		enable_openssl=no
	fi
fi

if test "x$enable_openssl" = "xyes"; then
	PKG_CHECK_MODULES(GSOAP, [gsoapssl])
else
	PKG_CHECK_MODULES(GSOAP, [gsoap])
fi

AM_CONDITIONAL(ENABLE_OPENSSL, test x"$enable_openssl" = "xyes")

GSOAP_PREFIX="$(pkg-config --variable=prefix gsoap)"
AC_SUBST(GSOAP_PREFIX)

AC_PATH_PROG([WSDL2H], [wsdl2h], [wsdl2h], [${GSOAP_PREFIX}/bin:${PATH}])
test -z "${WSDL2H}" && AC_MSG_ERROR([
*** You need the wsdl2h tool which is installed
*** from gSoap.
***
*** Either the location where you installed your gSoap is
*** not in your PATH or something is screwed up with your
*** gSoap installation
])

AC_PATH_PROG([SOAPCPP2], [soapcpp2], [soapcpp2], [${GSOAP_PREFIX}/bin:${PATH}])
test -z "${SOAPCPP2}" && AC_MSG_ERROR([
*** You need the soapcpp2 tool which is installed
*** from gSoap.
***
*** Either the location where you installed your gSoap is
*** not in your PATH or something is screwed up with your
*** gSoap installation
])

AC_OUTPUT([
Makefile
src/Makefile
src/discovery/Makefile
src/config/Makefile
src/discovery/config/Makefile

])

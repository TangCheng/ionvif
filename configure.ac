dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(ionvif, 0.1)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC


LT_INIT


PKG_CHECK_MODULES(IONVIF, [json-glib-1.0 glib-2.0 gobject-2.0 gsoap ])

G_SOAP_DIR="$(pkg-config --variable=prefix gsoap)"
AC_SUBST(G_SOAP_DIR)

AC_PATH_PROG([WSDL2H], [wsdl2h], [wsdl2h], [${G_SOAP_DIR}/bin:${PATH}])
test -z "${WSDL2H}" && AC_MSG_ERROR([
*** You need the wsdl2h tool which is installed
*** from gSoap.
***
*** Either the location where you installed your gSoap is
*** not in your PATH or something is screwed up with your
*** gSoap installation
])

AC_PATH_PROG([SOAPCPP2], [soapcpp2], [soapcpp2], [${G_SOAP_DIR}/bin:${PATH}])
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

])

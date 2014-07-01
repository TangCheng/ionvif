## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(IONVIF_CFLAGS) \
	$(OPENSSL_CFLAGS) \
	$(GSOAP_CFLAGS)

AM_CFLAGS = \
	 -Wall \
	 -g

bin_PROGRAMS =  \
	discovery

BUILT_SOURCES = \
	soapC.c \
	soapH.h \
	soapServer.c \
	soapClient.c \
	soapStub.h \
	RemoteDiscoveryBinding.nsmap \
	wsdd.nsmap

discovery_SOURCES =  \
	$(BUILT_SOURCES) \
	$(srcdir)/../gsoap-plugins/wsaapi.c \
	$(srcdir)/../gsoap-plugins/wsddapi.c \
	main.c

discovery_DEPENDENCIES = \
	onvif-generated.h

discovery_LDFLAGS = 

discovery_LDADD = $(IONVIF_LIBS) $(OPENSSL_LIBS) $(GSOAP_LIBS)

ONVIF_WSDL = \
	$(top_srcdir)/src/wsdl/ver10/network/wsdl/remotediscovery.wsdl

onvif-generated.h: $(ONVIF_WSDL)
	$(WSDL2H) -o $@ -c -s -t $(top_srcdir)/src/typemap.dat $^

GSOAP_IMPORT = \
	-I$(GSOAP_PREFIX)/share/gsoap/import

$(BUILT_SOURCES): onvif-generated.h
	$(SOAPCPP2) -c -2 -L -x onvif-generated.h $(GSOAP_IMPORT)

CLEANFILES = onvif-generated.h $(BUILT_SOURCES)

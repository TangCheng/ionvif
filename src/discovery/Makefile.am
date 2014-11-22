## Process this file with automake to produce Makefile.in

GSOAP_CFLAGS=$(shell PKG_CONFIG_PATH=$(top_builddir)/gsoap-2.8 pkg-config --cflags gsoap)
GSOAP_LDFLAGS=$(shell PKG_CONFIG_PATH=$(top_builddir)/gsoap-2.8 pkg-config --libs gsoap)

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

GSOAP_DIR = $(top_srcdir)/gsoap-2.8/gsoap

bindir = $(prefix)/ionvif-discovery
bin_PROGRAMS =  \
	ionvif-discovery

BUILT_SOURCES = \
	soapC.c \
	soapH.h \
	soapServer.c \
	soapClient.c \
	soapStub.h

NSMAP_FILES = \
	RemoteDiscoveryBinding.nsmap \
	wsdd.nsmap

ionvif_discovery_SOURCES =  \
	$(BUILT_SOURCES) \
	$(GSOAP_DIR)/plugin/wsaapi.c \
	$(GSOAP_DIR)/plugin/wsddapi.c \
	ipcam-ionvif-discovery.c \
	ipcam-ionvif-discovery.h \
	discovery-server.c \
	main.c

ionvif_discovery_CFLAGS = -I$(GSOAP_DIR) 

ionvif_discovery_LDFLAGS = -L$(top_builddir)/gsoap-2.8/gsoap -lgsoap

ionvif_discovery_LDADD = $(IONVIF_LIBS) $(OPENSSL_LIBS)

ionvif_discovery_DEPENDENCIES = \
	onvif-generated.h

WSDL2H = $(GSOAP_DIR)/bin/wsdl2h
SOAPCPP2 = $(GSOAP_DIR)/bin/soapcpp2

ONVIF_WSDL = \
	$(top_srcdir)/src/wsdl/ver10/network/wsdl/remotediscovery.wsdl

onvif-generated.h: $(ONVIF_WSDL)
	$(WSDL2H) -o $@ -c -s -x -y -P -p -t $(top_srcdir)/src/typemap.dat $(ONVIF_WSDL)

GSOAP_IMPORT = \
	-I$(GSOAP_DIR)/import

$(BUILT_SOURCES): onvif-generated.h
	$(SOAPCPP2) -c -2 -L -x -w -a $(top_srcdir)/src/onvif.h $(GSOAP_IMPORT)

CLEANFILES = onvif-generated.h $(BUILT_SOURCES) $(NSMAP_FILES)

SUBDIRS = \
	config

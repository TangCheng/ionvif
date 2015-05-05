## Process this file with automake to produce Makefile.in

## Created by Anjuta

GSOAP_CFLAGS=$(shell PKG_CONFIG_PATH=$(top_builddir)/gsoap-2.8 pkg-config --cflags gsoap)
GSOAP_LDFLAGS=$(shell PKG_CONFIG_PATH=$(top_builddir)/gsoap-2.8 pkg-config --libs gsoap)

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	-DLOCAL_STATE_DIR=\""$(localstatedir)"\" \
	$(IONVIF_CFLAGS) \
	$(OPENSSL_CFLAGS) \
	$(GSOAP_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

BUILT_SOURCES = \
	soapC.c \
	soapH.h \
	soapServer.c \
	soapStub.h

NSMAP_FILES = \
	DeviceBinding.nsmap \
	MediaBinding.nsmap \
	ImagingBinding.nsmap

GSOAP_DIR = $(top_srcdir)/gsoap-2.8/gsoap

bindir = $(prefix)/ionvif
bin_PROGRAMS = ionvif

ionvif_SOURCES = \
	onvif.h \
	$(BUILT_SOURCES) \
	$(GSOAP_DIR)/custom/duration.c \
    ipcam-ionvif-event-handler.c \
    ipcam-ionvif-event-handler.h \
	onvif-auth.c \
	onvif-auth.h \
	soapTester.c \
	onvif_impl.c \
	onvif-device-mgmt.c \
	onvif-network-mgmt.c \
	onvif-user-mgmt.c \
	onvif-media.c \
	onvif-server.c \
	ionvif.c \
	ionvif.h \
	main.c

ionvif_DEPENDENCIES = \
	onvif-generated.h

ionvif_CFLAGS = -I$(GSOAP_DIR)

ionvif_LDADD = $(IONVIF_LIBS) $(OPENSSL_LIBS)

if ENABLE_OPENSSL
ionvif_SOURCES += \
	$(GSOAP_DIR)/plugin/mecevp.c \
	$(GSOAP_DIR)/plugin/smdevp.c \
	$(GSOAP_DIR)/plugin/wsseapi.c \
	$(GSOAP_DIR)/plugin/wsaapi.c
ionvif_CFLAGS += -DWITH_OPENSSL
ionvif_LDFLAGS = -L$(top_builddir)/gsoap-2.8/gsoap -lgsoapssl
else
ionvif_LDFLAGS = -L$(top_builddir)/gsoap-2.8/gsoap -lgsoap
endif

WSDL2H = $(GSOAP_DIR)/bin/wsdl2h
SOAPCPP2 = $(GSOAP_DIR)/bin/soapcpp2

$(WSDL2H): $(SOAPCPP2)
	$(MAKE) -C $(GSOAP_DIR)/wsdl -f MakefileManual

$(SOAPCPP2):
	$(MAKE) -C $(GSOAP_DIR)/src -f MakefileManual

ONVIF_WSDL = \
	$(srcdir)/wsdl/ver10/device/wsdl/devicemgmt.wsdl \
	$(srcdir)/wsdl/ver10/media/wsdl/media.wsdl \
	$(srcdir)/wsdl/ver20/imaging/wsdl/imaging.wsdl

onvif-generated.h: $(ONVIF_WSDL) $(WSDL2H)
	$(WSDL2H) -o $@ -c -s -x -y -P -p -t $(top_srcdir)/src/typemap.dat $(ONVIF_WSDL)

GSOAP_IMPORT = \
	-I$(GSOAP_DIR) \
	-I$(GSOAP_DIR)/import

$(BUILT_SOURCES): onvif-generated.h $(SOAPCPP2)
	$(SOAPCPP2) -c -L -x -w -a -S $(top_srcdir)/src/onvif.h -I. $(GSOAP_IMPORT)

CLEANFILES = onvif-generated.h $(BUILT_SOURCES) $(NSMAP_FILES)

SUBDIRS = \
	discovery \
	config

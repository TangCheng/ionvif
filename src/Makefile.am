## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
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

GSOAP_DATA_DIR = $(GSOAP_PREFIX)/share/gsoap

bindir = $(prefix)/ionvif
bin_PROGRAMS = ionvif

ionvif_SOURCES = \
	onvif.h \
	$(BUILT_SOURCES) \
	$(GSOAP_DATA_DIR)/custom/duration.c \
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

if ENABLE_OPENSSL
ionvif_SOURCES += \
	$(GSOAP_DATA_DIR)/plugin/mecevp.c \
	$(GSOAP_DATA_DIR)/plugin/smdevp.c \
	$(GSOAP_DATA_DIR)/plugin/wsseapi.c \
	$(GSOAP_DATA_DIR)/plugin/wsaapi.c
endif

ionvif_LDFLAGS =

ionvif_LDADD = $(IONVIF_LIBS) $(OPENSSL_LIBS) $(GSOAP_LIBS)

ONVIF_WSDL = \
	wsdl/ver10/device/wsdl/devicemgmt.wsdl \
	wsdl/ver10/media/wsdl/media.wsdl \
	wsdl/ver20/imaging/wsdl/imaging.wsdl

onvif-generated.h: $(ONVIF_WSDL)
	$(WSDL2H) -o $@ -c -s -x -y -P -p -t $(top_srcdir)/src/typemap.dat $^

GSOAP_IMPORT = \
	-I$(GSOAP_DATA_DIR) \
	-I$(GSOAP_DATA_DIR)/import

$(BUILT_SOURCES): onvif-generated.h
	$(SOAPCPP2) -c -L -x -w -a -S $(top_srcdir)/src/onvif.h -I. $(GSOAP_IMPORT)

CLEANFILES = onvif-generated.h $(BUILT_SOURCES) $(NSMAP_FILES)

SUBDIRS = \
	discovery \
	config

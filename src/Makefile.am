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

bindir = $(prefix)/ionvif
bin_PROGRAMS = ionvif

BUILT_SOURCES = \
	soapC.c \
	soapH.h \
	soapServer.c \
	soapStub.h \
	DeviceBinding.nsmap \
	MediaBinding.nsmap \
	ImagingBinding.nsmap

ionvif_SOURCES = \
	onvif.h \
	$(BUILT_SOURCES) \
	duration.c \
	onvif-authentication.c \
	onvif-authentication.h \
	soapTester.c \
	onvif_impl.c \
	device_manage.c \
	user_mgmt.c \
	media.c \
	onvif-server.c \
	ionvif.c \
	ionvif.h \
	main.c

ionvif_DEPENDENCIES = \
	onvif-generated.h

if ENABLE_OPENSSL
ionvif_SOURCES += \
	gsoap-plugins/mecevp.c \
	gsoap-plugins/smdevp.c \
	gsoap-plugins/wsseapi.c \
	gsoap-plugins/wsaapi.c
endif

ionvif_LDFLAGS = 

ionvif_LDADD = $(IONVIF_LIBS) $(OPENSSL_LIBS) $(GSOAP_LIBS)

ONVIF_WSDL = \
	wsdl/ver10/device/wsdl/devicemgmt.wsdl \
	wsdl/ver10/media/wsdl/media.wsdl \
	wsdl/ver20/imaging/wsdl/imaging.wsdl

onvif-generated.h: $(ONVIF_WSDL)
	$(WSDL2H) -o $@ -c -s -t $(srcdir)/typemap.dat $^

GSOAP_IMPORT = \
	-I$(GSOAP_PREFIX)/share/gsoap \
	-I$(GSOAP_PREFIX)/share/gsoap/import

$(BUILT_SOURCES): onvif-generated.h
	$(SOAPCPP2) -c -2 -L -x -w -S $(srcdir)/onvif.h -I. $(GSOAP_IMPORT)

CLEANFILES = onvif-generated.h $(BUILT_SOURCES)

SUBDIRS = \
	discovery \
	config

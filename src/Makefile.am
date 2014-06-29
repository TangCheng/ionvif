## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(IONVIF_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = ionvif

ionvif_SOURCES = \
	onvif.h \
	$(BUILT_SOURCES) \
	duration.c \
	soapTester.c \
	onvif_impl.c \
	device_manage.c \
	media.c \
	main.c

ionvif_LDFLAGS = 

ionvif_LDADD = $(IONVIF_LIBS)

ONVIF_WSDL = \
	wsdl/ver10/device/wsdl/devicemgmt.wsdl \
	wsdl/ver10/media/wsdl/media.wsdl \
	wsdl/ver20/imaging/wsdl/imaging.wsdl

BUILT_SOURCES = \
	soapC.c \
	soapH.h \
	soapServer.c \
	soapStub.h \
	DeviceBinding.nsmap \
	MediaBinding.nsmap \
	ImagingBinding.nsmap

onvif.h: $(ONVIF_WSDL)
	$(WSDL2H) -o $@ -c -s -t $(srcdir)/typemap.dat $^

$(BUILT_SOURCES): onvif.h
	$(SOAPCPP2) -c -2 -L -x -S onvif.h -I$(G_SOAP_DIR)/share/gsoap -I$(G_SOAP_DIR)/share/gsoap/import -I.

CLEANFILES = onvif.h $(BUILT_SOURCES)

SUBDIRS = \
	discovery

/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2014 Watson Xu <xuhuashan@gmail.com>
 * 
 * ionvif is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * ionvif is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "soapH.h"
#include "onvif-server.h"
#include "onvif_impl.h"
#include "ionvif.h"


struct mime_server_handle
{
	char *key;	/* file name */
	FILE *fd;	/* file fd */
};

static void *mime_server_write_open(struct soap *soap, void *unused_handle,
		const char *id, const char *type, const char *description,
		enum soap_mime_encoding encoding)
{ /* Note: the 'unused_handle' is always NULL */
	/* Return NULL without setting soap->error if we don't want to use the streaming callback for this DIME attachment */
	const char *file;
	struct mime_server_handle *handle = soap_malloc(soap,
			sizeof(struct mime_server_handle));
	if (!handle) {
		soap->error = SOAP_EOM;
		return NULL;
	}
	/* Create a new file */
	file = tempnam(LOCAL_STATE_DIR, "data");
	/* The file name is also the key */
	handle->key = soap_strdup(soap, file);
	handle->fd = fopen(file, "wb");
	free((void*) file);
	if (!handle->fd) {
		soap->error = soap_sender_fault(soap,
				"Cannot save data to file", handle->key);
		soap->errnum = errno; /* get reason */
		return NULL;
	}
	fprintf(stderr, "Saving file %s type %s\n", handle->key,
			type ? type : "");
	return (void*) handle;
}

static void mime_server_write_close(struct soap *soap, void *handle)
{
	fclose(((struct mime_server_handle*) handle)->fd);
}

static int mime_server_write(struct soap *soap, void *handle, const char *buf,
		size_t len)
{
	FILE *fd = ((struct mime_server_handle*) handle)->fd;
	while (len) {
		size_t nwritten = fwrite(buf, 1, len, fd);
		if (!nwritten) {
			soap->errnum = errno;
			return SOAP_EOF;
		}
		len -= nwritten;
		buf += nwritten;
	}
	return SOAP_OK;
}

gpointer onvif_server_thread_func(gpointer data)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)data;
	SOAP_SOCKET m, s;
	int port = 8080;
	struct soap soap;

	soap_init1(&soap, SOAP_ENC_MTOM);

#include "DeviceBinding.nsmap"
	soap_set_namespaces(&soap, namespaces);

	soap.user = ionvif;

	/* reuse address */
 	soap.bind_flags = SO_REUSEADDR;


	m = soap_bind(&soap, NULL, port, 100);
	if (!soap_valid_socket(m)) {
		g_warn_if_reached();
		return NULL;
	}

	printf("socket bind success %d\n", m);

	soap.fmimewriteopen = mime_server_write_open;
	soap.fmimewriteclose = mime_server_write_close;
	soap.fmimewrite = mime_server_write;

	for (;!ipcam_ionvif_is_terminating(ionvif);) {
		s = soap_accept(&soap);
		printf("socket connect %d\n", s);
		if (!soap_valid_socket(s)) {
			soap_print_fault(&soap, stderr);

			break;
		}
		if (soap_serve(&soap) != SOAP_OK) {
			soap_print_fault(&soap, stderr);
		}
		soap_destroy(&soap);
		soap_end(&soap);
	}

	soap_done(&soap);
	return 0;
}


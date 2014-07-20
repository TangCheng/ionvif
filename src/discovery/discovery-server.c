/*
 * main.c
 *
 * Copyright (C) 2014 - Watson Xu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib-object.h>
#include "soapStub.h"
#include "RemoteDiscoveryBinding.nsmap"
#include <plugin/wsddapi.h>


/*WS-Discovery specialization address and port of UDP*/
#define ONVIF_MULTICAST_GROUP ("239.255.255.250")  
#define ONVIF_DISCOVERY_PORT  (3702)
#define ONVIF_SERVICE_PORT    (8080)

/*
char  g_scopes[] = "onvif://www.onvif.org/Profile/Streaming \
				onvif://www.onvif.org/model/C5F0S7Z0N1P0L0V0 \
				onvif://www.onvif.org/name/IPCAM \
				onvif://www.onvif.org/location/country/china"; */

char  g_scopes[] = "onvif://www.onvif.org/name/IPCAM";

gpointer onvif_discovery_server_thread_func(gpointer data)
{
	SOAP_SOCKET m, s;
	struct soap soap;

	soap_init1(&soap, SOAP_IO_UDP);

	m = soap_bind(&soap, NULL,  ONVIF_DISCOVERY_PORT, 100);
 	/* reuse address */
 	soap.bind_flags = SO_REUSEADDR;
	if (!soap_valid_socket(m)) {
		soap_print_fault(&soap, stderr);
		exit(-1);
	}
	/* optionally join a multicast group */
	if (ONVIF_MULTICAST_GROUP) { 
		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = inet_addr(ONVIF_MULTICAST_GROUP);
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if (setsockopt(soap.socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
			printf("setsockopt failed\n");
	      	exit(-1);
		}
	}
	printf("socket bind success %d\n", m);
	for (;;) {
		printf("socket connect %d\n", s);
		s = soap_accept(&soap);
		if (!soap_valid_socket(s)) {
			soap_print_fault(&soap, stderr);
			exit(-1);
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


SOAP_FMAC5 int SOAP_FMAC6 SOAP_ENV__Fault(struct soap* soap, char *faultcode, char *faultstring, char *faultactor, struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *SOAP_ENV__Code, struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node, char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail) {
	printf("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}


SOAP_FMAC5 int SOAP_FMAC6 __tdn__Hello(struct soap* soap, struct wsdd__HelloType tdn__Hello, struct wsdd__ResolveType *tdn__HelloResponse) {
	printf("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tdn__Bye(struct soap* soap, struct wsdd__ByeType tdn__Bye, struct wsdd__ResolveType *tdn__ByeResponse) {
	printf("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tdn__Probe(struct soap* soap, struct wsdd__ProbeType tdn__Probe, struct wsdd__ProbeMatchesType *tdn__ProbeResponse) {
	printf("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
    printf("%s,%d\n",__FUNCTION__, __LINE__);
}
 
void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
    printf("%s,%d\n",__FUNCTION__, __LINE__);
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID,
		const char *ReplyTo, const char *Types, const char *Scopes,
		const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
	char *xaddrs = NULL;
	char *ep_ref = NULL;

	printf("%s,%d\n", __FUNCTION__, __LINE__);
	printf("MessageID:%s\n", MessageID);
	printf("ReplyTo:%s\n", ReplyTo);
	printf("Types:%s\n", Types);
	printf("Scopes:%s\n", Scopes);
	printf("MatchBy:%s\n", MatchBy);
	printf("\n");

	soap->header->wsa__RelatesTo = (struct wsa__Relationship*) soap_malloc(
			soap, sizeof(struct wsa__Relationship));

	soap->header->wsa__To = soap_strdup(soap, ReplyTo);
	soap->header->wsa__RelatesTo->__item = soap_strdup(soap, MessageID);
	soap->header->wsa__RelatesTo->RelationshipType = NULL;
	soap->header->wsa__RelatesTo->__anyAttribute = NULL;
	soap->header->wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";
	soap->header->wsa__ReplyTo = NULL;

	soap_wsdd_init_ProbeMatches(soap, matches);

	asprintf(&xaddrs, "http://%s:%d/onvif/device_service", 
	         "192.168.1.5",
	         ONVIF_SERVICE_PORT);
	ep_ref = "urn:uuid:464A4854-4656-5242-4530-313035394100";
	soap_wsdd_add_ProbeMatch(soap, matches,
				soap_strdup(soap, ep_ref),
				"tdn:NetworkVideoTransmitter",
				g_scopes,
				NULL,
				soap_strdup(soap, xaddrs),
				10);
	free(xaddrs);

	return SOAP_WSDD_MANAGED;
}
 
void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
    printf("%s,%d\n",__FUNCTION__, __LINE__);
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    printf("%s,%d\n",__FUNCTION__, __LINE__);
    return 0;
}
 
void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
    printf("%s,%d\n",__FUNCTION__, __LINE__);
}

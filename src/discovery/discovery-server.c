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
#include <request_message.h>
#include "soapStub.h"
#include "RemoteDiscoveryBinding.nsmap"
#include <plugin/wsddapi.h>

#include "ipcam-ionvif-discovery.h"

#define DBG_PRINT(x...)

/*WS-Discovery specialization address and port of UDP*/
#define ONVIF_MULTICAST_GROUP ("239.255.255.250")  
#define ONVIF_DISCOVERY_PORT  (3702)
#define ONVIF_SERVICE_PORT    (8080)

struct NetworkParameters
{
	guint http_port;
	struct in_addr ipaddr;
	guint mac_addr[6];
};

/*
char  g_scopes[] = "onvif://www.onvif.org/Profile/Streaming \
				onvif://www.onvif.org/model/C5F0S7Z0N1P0L0V0 \
				onvif://www.onvif.org/name/IPCAM \
				onvif://www.onvif.org/location/country/china"; */

char  g_scopes[] = "onvif://www.onvif.org/name/ipcam";

gpointer onvif_discovery_server_thread_func(gpointer data)
{
    IpcamIOnvifDiscovery *ionvif_discovery = IPCAM_IONVIF_DISCOVERY(data);
	SOAP_SOCKET m, s;
	struct soap soap;

	soap_init1(&soap, SOAP_IO_UDP);

    soap.user = ionvif_discovery;
    /* reuse address */
 	soap.bind_flags = SO_REUSEADDR;

	m = soap_bind(&soap, NULL,  ONVIF_DISCOVERY_PORT, 100);
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
			perror("setsockopt failed:");
			exit(-1);
		}
	}

    for (;;) {
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
	DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}


SOAP_FMAC5 int SOAP_FMAC6 __tdn__Hello(struct soap* soap, struct wsdd__HelloType tdn__Hello, struct wsdd__ResolveType *tdn__HelloResponse) {
	DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tdn__Bye(struct soap* soap, struct wsdd__ByeType tdn__Bye, struct wsdd__ResolveType *tdn__ByeResponse) {
	DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __tdn__Probe(struct soap* soap, struct wsdd__ProbeType tdn__Probe, struct wsdd__ProbeMatchesType *tdn__ProbeResponse) {
	DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
	return 0;
}

void wsdd_event_Hello(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int MetadataVersion)
{
    DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
}
 
void wsdd_event_Bye(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, const char *EndpointReference, const char *Types, const char *Scopes, const char *MatchBy, const char *XAddrs, unsigned int *MetadataVersion)
{
    DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
}

static gboolean
onvif_invocate_action(IpcamIOnvifDiscovery *ionvif_discovery, const char *action,
                               JsonNode *request, JsonNode **response)
{
	const gchar *token;
	IpcamRequestMessage *req_msg;
	IpcamMessage *resp_msg;
	gboolean ret = FALSE;

	token = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif_discovery), "token");

	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", action,
	                       "body", request,
	                       NULL);

	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif_discovery),
	                            IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, NULL, 10);

	ret = ipcam_base_app_wait_response(IPCAM_BASE_APP(ionvif_discovery),
	                                   ipcam_request_message_get_id(req_msg),
	                                   5000, &resp_msg);
	if (ret)
	{
		JsonNode *resp_body;

        if (response) {
            g_object_get(G_OBJECT(resp_msg), "body", &resp_body, NULL);
            *response = json_node_copy(resp_body);
        }

		g_object_unref(resp_msg);
	}

	g_object_unref(req_msg);

	return ret;
}

static gboolean get_network_parameters(IpcamIOnvifDiscovery *ionvif_discovery,
                                       struct NetworkParameters *param)
{
	JsonBuilder *builder;
	JsonNode *response;

	/* Request the Network setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "address");
	json_builder_add_string_value(builder, "port");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	if (onvif_invocate_action(ionvif_discovery, "get_network", json_builder_get_root(builder), &response)) {
		JsonObject *items_obj;

		items_obj = json_object_get_object_member(json_node_get_object(response), "items");
		if (json_object_has_member(items_obj, "port"))
		{
			JsonObject *port_obj = json_object_get_object_member(items_obj, "port");
			if (json_object_has_member(port_obj, "http"))
			{
				param->http_port = json_object_get_int_member(port_obj, "http");
			}
		}

		if (json_object_has_member(items_obj, "address"))
		{
			JsonObject *addr_obj = json_object_get_object_member(items_obj, "address");
			if (json_object_has_member(addr_obj, "ipaddr"))
			{
				inet_aton(json_object_get_string_member(addr_obj, "ipaddr"), &param->ipaddr);
			}
		}
	}
	g_object_unref(builder);

	/* Request the mac address */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "hwaddr");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	if (onvif_invocate_action(ionvif_discovery, "get_base_info", json_builder_get_root(builder), &response)) {
		JsonObject *items_obj;

		items_obj = json_object_get_object_member(json_node_get_object(response), "items");
		if (json_object_has_member(items_obj, "hwaddr"))
		{
			int *mac_addr = param->mac_addr;
			const gchar *mac_addr_str = json_object_get_string_member(items_obj, "hwaddr");

			sscanf(mac_addr_str, "%x:%x:%x:%x:%x:%x",
			       &mac_addr[0], &mac_addr[1], &mac_addr[2],
			       &mac_addr[3], &mac_addr[4], &mac_addr[5]);
		}
	}
	g_object_unref(builder);

	return TRUE;
}

soap_wsdd_mode wsdd_event_Probe(struct soap *soap, const char *MessageID,
		const char *ReplyTo, const char *Types, const char *Scopes,
		const char *MatchBy, struct wsdd__ProbeMatchesType *matches)
{
    IpcamIOnvifDiscovery *ionvif_discovery = IPCAM_IONVIF_DISCOVERY(soap->user);
	struct NetworkParameters param;
	char *xaddrs = NULL;
	char *ep_ref = NULL;

    g_assert(IPCAM_IS_IONVIF_DISCOVERY(ionvif_discovery));

#if 0
	DBG_PRINT("%s,%d\n", __FUNCTION__, __LINE__);
	DBG_PRINT("MessageID:%s\n", MessageID);
	DBG_PRINT("ReplyTo:%s\n", ReplyTo);
	DBG_PRINT("Types:%s\n", Types);
	DBG_PRINT("Scopes:%s\n", Scopes);
	DBG_PRINT("MatchBy:%s\n", MatchBy);
	DBG_PRINT("\n");
#endif

	soap->header->wsa__RelatesTo = (struct wsa__Relationship*) soap_malloc(
			soap, sizeof(struct wsa__Relationship));

	soap->header->wsa__To = soap_strdup(soap, ReplyTo);
	soap->header->wsa__RelatesTo->__item = soap_strdup(soap, MessageID);
	soap->header->wsa__RelatesTo->RelationshipType = NULL;
	soap->header->wsa__RelatesTo->__anyAttribute = NULL;
	soap->header->wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";
	soap->header->wsa__ReplyTo = NULL;

	soap_wsdd_init_ProbeMatches(soap, matches);

	param.http_port = 80;
	inet_aton("127.0.0.1", &param.ipaddr);
	memset(param.mac_addr, 0, sizeof(param.mac_addr));

	if (!get_network_parameters(ionvif_discovery, &param))
		g_warning("Get network parameter failed.\n");

	if (param.http_port == 80)
    {
        asprintf(&xaddrs, "http://%s/onvif/device_service", 
                 inet_ntoa(param.ipaddr));
    }
    else
    {
        asprintf(&xaddrs, "http://%s:%d/onvif/device_service", 
                 inet_ntoa(param.ipaddr),
                 param.http_port);
    }
    ep_ref = g_malloc0(64);
	g_snprintf(ep_ref, 64, "urn:uuid:464A4854-4656-5242-4530-%02X%02X%02X%02X%02X%02X",
               param.mac_addr[0], param.mac_addr[1], param.mac_addr[2],
	           param.mac_addr[3], param.mac_addr[4], param.mac_addr[5]);
	soap_wsdd_add_ProbeMatch(soap, matches,
				soap_strdup(soap, ep_ref),
				"tdn:NetworkVideoTransmitter",
				g_scopes,
				NULL,
				soap_strdup(soap, xaddrs),
				10);
	free(xaddrs);
    g_free(ep_ref);

	return SOAP_WSDD_MANAGED;
}
 
void wsdd_event_ProbeMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ProbeMatchesType *matches)
{
    DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
}

soap_wsdd_mode wsdd_event_Resolve(struct soap *soap, const char *MessageID, const char *ReplyTo, const char *EndpointReference, struct wsdd__ResolveMatchType *match)
{
    DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
    return 0;
}
 
void wsdd_event_ResolveMatches(struct soap *soap, unsigned int InstanceId, const char *SequenceId, unsigned int MessageNumber, const char *MessageID, const char *RelatesTo, struct wsdd__ResolveMatchType *match)
{
    DBG_PRINT("%s,%d\n",__FUNCTION__, __LINE__);
}

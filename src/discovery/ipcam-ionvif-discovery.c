/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ipcam-ionvif-discovery.c
 * Copyright (C) 2014 Watson Xu <watson@watsons-pc>
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

#include <arpa/inet.h>
#include <json-glib/json-glib.h>
#include <request_message.h>
#include "discovery-server.h"
#include "ipcam-ionvif-discovery.h"
#include "ipcam-ionvif-discovery-event-handler.h"

struct _IpcamIOnvifDiscoveryPrivate
{
	GThread *discovery_thread;
	gboolean terminating;
	guint http_port;
	struct in_addr server_addr;
	gchar *mac_addr;
};

G_DEFINE_TYPE (IpcamIOnvifDiscovery, ipcam_ionvif_discovery, IPCAM_BASE_APP_TYPE);

static void
ipcam_ionvif_discovery_init (IpcamIOnvifDiscovery *ipcam_ionvif_discovery)
{
	ipcam_ionvif_discovery->priv = G_TYPE_INSTANCE_GET_PRIVATE (ipcam_ionvif_discovery, IPCAM_TYPE_IONVIF_DISCOVERY, IpcamIOnvifDiscoveryPrivate);

	ipcam_ionvif_discovery->priv->discovery_thread = NULL;
	ipcam_ionvif_discovery->priv->terminating = FALSE;
	inet_aton("127.0.0.1", &ipcam_ionvif_discovery->priv->server_addr);
	ipcam_ionvif_discovery->priv->http_port = 80;
}

static void
ipcam_ionvif_discovery_finalize (GObject *object)
{
	IpcamIOnvifDiscovery *ionvif_discovery = IPCAM_IONVIF_DISCOVERY(object);
	IpcamIOnvifDiscoveryPrivate *priv = ionvif_discovery->priv;

	priv->terminating = TRUE;
	g_thread_join(priv->discovery_thread);

	g_free(priv->mac_addr);

	G_OBJECT_CLASS (ipcam_ionvif_discovery_parent_class)->finalize (object);
}

static void ipcam_ionvif_discovery_before_start(IpcamBaseService *base_service);
static void ipcam_ionvif_discovery_in_loop(IpcamBaseService *base_service);

static void
ipcam_ionvif_discovery_class_init (IpcamIOnvifDiscoveryClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IpcamIOnvifDiscoveryPrivate));

	object_class->finalize = ipcam_ionvif_discovery_finalize;

	IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
	base_service_class->before = ipcam_ionvif_discovery_before_start;
	base_service_class->in_loop = ipcam_ionvif_discovery_in_loop;
}


void ipcam_ionvif_discovery_update_network_setting(IpcamIOnvifDiscovery *ionvif_discovery, JsonNode *body)
{
    JsonObject *items_obj = json_object_get_object_member(json_node_get_object(body), "items");
	IpcamIOnvifDiscoveryPrivate *priv = ionvif_discovery->priv;

    if (json_object_has_member(items_obj, "port"))
	{
        JsonObject *port_obj = json_object_get_object_member(items_obj, "port");
        if (json_object_has_member(port_obj, "http"))
		{
			priv->http_port = json_object_get_int_member(port_obj, "http");
		}
    }

	if (json_object_has_member(items_obj, "address"))
	{
		JsonObject *addr_obj = json_object_get_object_member(items_obj, "address");
		if (json_object_has_member(addr_obj, "ipaddr"))
		{
			inet_aton(json_object_get_string_member(addr_obj, "ipaddr"), &priv->server_addr);
		}
	}
}

static ipcam_ionvif_discovery_update_mac_addr(IpcamIOnvifDiscovery *ionvif_discovery, JsonNode *body)
{
	JsonObject *items_obj = json_object_get_object_member(json_node_get_object(body), "items");
	IpcamIOnvifDiscoveryPrivate *priv = ionvif_discovery->priv;

    if (json_object_has_member(items_obj, "hwaddr"))
	{
		const gchar *mac_addr = json_object_get_string_member(items_obj, "hwaddr");
		gchar **mac_array = g_strsplit(mac_addr, ":", 0);
		
        priv->mac_addr = g_malloc0(13);
		g_snprintf(priv->mac_addr, 13, "%s%s%s%s%s%s%s",
				   mac_array[0], mac_array[1], mac_array[2],
				   mac_array[3], mac_array[4], mac_array[5]);
		g_strfreev(mac_array);
    }
}

static void network_message_handler(GObject *obj, IpcamMessage *msg, gboolean timeout)
{
	IpcamIOnvifDiscovery *ionvif_discovery = IPCAM_IONVIF_DISCOVERY(obj);
	g_assert(IPCAM_IS_IONVIF_DISCOVERY(ionvif_discovery));

	if (!timeout && msg)
	{
		JsonNode *body;
		gchar *action;
		g_object_get(msg, "action", &action, "body", &body, NULL);
		if (body)
		{
			if (g_str_equal(action, "get_network"))
			{
				ipcam_ionvif_discovery_update_network_setting(ionvif_discovery, body);
			}
			else if (g_str_equal(action, "get_base_info"))
			{
				ipcam_ionvif_discovery_update_mac_addr(ionvif_discovery, body);
			}
			else
			{
				g_warn_if_reached();
			}
		}
		g_free(action);
	}
}

static void ipcam_ionvif_discovery_before_start(IpcamBaseService *base_service)
{
	IpcamIOnvifDiscovery *ionvif_discovery = IPCAM_IONVIF_DISCOVERY(base_service);
	IpcamIOnvifDiscoveryPrivate *priv = ionvif_discovery->priv;
	JsonBuilder *builder;
	const gchar *token = "discovery_token";
	IpcamRequestMessage *req_msg;

	priv->discovery_thread = g_thread_new("onvif-discovery-server",
	                                      onvif_discovery_server_thread_func,
	                                      ionvif_discovery);

	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif_discovery), "set_network",
	                                       IPCAM_TYPE_IONVIF_DISCOVERY_EVENT_HANDLER);

	/* Request the Network setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "address");
	json_builder_add_string_value(builder, "port");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_network",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif_discovery), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token,
	                            network_message_handler, 5);

	g_object_unref(req_msg);
	g_object_unref(builder);

	/* Request the mac address */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "hwaddr");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_base_info",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif_discovery), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token,
	                            network_message_handler, 5);

	g_object_unref(req_msg);
	g_object_unref(builder);
}

static void ipcam_ionvif_discovery_in_loop(IpcamBaseService *base_service)
{
}

gboolean ipcam_ionvif_discovery_is_terminating(IpcamIOnvifDiscovery *ionvif_discovery)
{
	return ionvif_discovery->priv->terminating;
}

struct in_addr ipcam_ionvif_discovery_get_server_addr(IpcamIOnvifDiscovery *ionvif_discovery)
{
	return ionvif_discovery->priv->server_addr;
}

guint ipcam_ionvif_discovery_get_server_port(IpcamIOnvifDiscovery *ionvif_discovery)
{
	return ionvif_discovery->priv->http_port;
}

const gchar *ipcam_ionvif_discovery_get_mac_addr(IpcamIOnvifDiscovery *ionvif_discovery)
{
	return (const gchar *)ionvif_discovery->priv->mac_addr;
}

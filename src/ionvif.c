/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * ionvif.c
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
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <json-glib/json-glib.h>
#include <request_message.h>
#include "ionvif.h"
#include "onvif-server.h"
#include "ipcam-ionvif-event-handler.h"


struct _IpcamIOnvifPrivate
{
	GThread *service_thread;
	gboolean terminating;

	GHashTable *cached_users_hash;
	GHashTable *cached_property_hash;
	GMutex mutex;
};




G_DEFINE_TYPE (IpcamIOnvif, ipcam_ionvif, IPCAM_BASE_APP_TYPE);

static void config_key_destroy_func(gpointer key)
{
	g_free(key);
}

static void config_value_destroy_func(gpointer value)
{
	g_free(value);
}

static void
ipcam_ionvif_init (IpcamIOnvif *ipcam_ionvif)
{
	ipcam_ionvif->priv = G_TYPE_INSTANCE_GET_PRIVATE (ipcam_ionvif, IPCAM_TYPE_IONVIF, IpcamIOnvifPrivate);

	ipcam_ionvif->priv->service_thread = NULL;
	ipcam_ionvif->priv->terminating = FALSE;

	ipcam_ionvif->priv->cached_property_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
	                                                                 config_key_destroy_func,
	                                                                 config_value_destroy_func);
	ipcam_ionvif->priv->cached_users_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
																  config_key_destroy_func,
																  config_value_destroy_func);
	g_mutex_init(&ipcam_ionvif->priv->mutex);
}

static void
ipcam_ionvif_finalize (GObject *object)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(object);
	IpcamIOnvifPrivate *priv = ionvif->priv;

	priv->terminating = TRUE;
	g_thread_join(priv->service_thread);
	g_hash_table_destroy(priv->cached_property_hash);
	g_hash_table_destroy(priv->cached_users_hash);
	g_mutex_clear(&priv->mutex);

	G_OBJECT_CLASS (ipcam_ionvif_parent_class)->finalize (object);
}

static void ipcam_ionvif_before_start(IpcamBaseService *base_service);
static void ipcam_ionvif_in_loop(IpcamBaseService *base_service);

static void
ipcam_ionvif_class_init (IpcamIOnvifClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (IpcamIOnvifPrivate));

	object_class->finalize = ipcam_ionvif_finalize;

	IpcamBaseServiceClass *base_service_class = IPCAM_BASE_SERVICE_CLASS(klass);
	base_service_class->before = ipcam_ionvif_before_start;
	base_service_class->in_loop = ipcam_ionvif_in_loop;
}


void ipcam_ionvif_update_base_info_setting(IpcamIOnvif *ionvif, JsonNode *body)
{
    JsonObject *items_obj = json_object_get_object_member(json_node_get_object(body), "items");
	GList *members, *item;

	members = json_object_get_members(items_obj);
	for (item = g_list_first(members); item; item = g_list_next(item)) {
		const gchar *name = (const gchar *)item->data;
		gchar *key;
		if (asprintf(&key, "base_info:%s", (const gchar *)item->data) > 0) {
			const gchar *value = json_object_get_string_member(items_obj, name);
			ipcam_ionvif_set_string_property(ionvif, key, value);
			g_free(key);
		}
	}
	g_list_free(members);
}

static void base_info_message_handler(GObject *obj, IpcamMessage *msg, gboolean timeout)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(obj);
	g_assert(IPCAM_IS_IONVIF(ionvif));

	if (!timeout && msg) {
		JsonNode *body;
		g_object_get(msg, "body", &body, NULL);
		if (body)
			ipcam_ionvif_update_base_info_setting(ionvif, body);
	}
}


void ipcam_ionvif_update_network_setting(IpcamIOnvif *ionvif, JsonNode *body)
{
    JsonObject *items_obj = json_object_get_object_member(json_node_get_object(body), "items");

    if (json_object_has_member(items_obj, "method"))
        ipcam_ionvif_set_string_property(ionvif, "network:method",
										 json_object_get_string_member(items_obj, "method"));
    if (json_object_has_member(items_obj, "address")) {
        JsonObject *addr_obj = json_object_get_object_member(items_obj, "address");

		if (json_object_has_member(addr_obj, "hwaddr"))
            ipcam_ionvif_set_string_property(ionvif, "network:address:hwaddr",
                                             json_object_get_string_member(addr_obj, "hwaddr"));
		if (json_object_has_member(addr_obj, "ipaddr"))
            ipcam_ionvif_set_string_property(ionvif, "network:address:ipaddr",
                                             json_object_get_string_member(addr_obj, "ipaddr"));
        if (json_object_has_member(addr_obj, "netmask"))
            ipcam_ionvif_set_string_property(ionvif, "network:address:netmask",
                                             json_object_get_string_member(addr_obj, "netmask"));
        if (json_object_has_member(addr_obj, "gateway"))
            ipcam_ionvif_set_string_property(ionvif, "network:address:gateway",
                                             json_object_get_string_member(addr_obj, "gateway"));
        if (json_object_has_member(addr_obj, "dns1"))
            ipcam_ionvif_set_string_property(ionvif, "network:address:dns1",
                                             json_object_get_string_member(addr_obj, "dns1"));
        if (json_object_has_member(addr_obj, "dns2"))
            ipcam_ionvif_set_string_property(ionvif, "network:address:dns2",
                                             json_object_get_string_member(addr_obj, "dns2"));
    }
    if (json_object_has_member(items_obj, "pppoe")) {
        JsonObject *pppoe_obj = json_object_get_object_member(items_obj, "pppoe");

        if (json_object_has_member(pppoe_obj, "username"))
            ipcam_ionvif_set_string_property(ionvif, "network:pppoe:username",
                                             json_object_get_string_member(pppoe_obj, "username"));
        if (json_object_has_member(pppoe_obj, "password"))
            ipcam_ionvif_set_string_property(ionvif, "network:pppoe:password",
                                             json_object_get_string_member(pppoe_obj, "password"));
    }
    if (json_object_has_member(items_obj, "port")) {
        JsonObject *port_obj = json_object_get_object_member(items_obj, "port");

        if (json_object_has_member(port_obj, "http"))
            ipcam_ionvif_set_int_property(ionvif, "network:port:http",
                                          json_object_get_int_member(port_obj, "http"));
		if (json_object_has_member(port_obj, "ftp"))
            ipcam_ionvif_set_int_property(ionvif, "network:port:ftp",
                                          json_object_get_int_member(port_obj, "ftp"));
        if (json_object_has_member(port_obj, "rtsp"))
            ipcam_ionvif_set_int_property(ionvif, "network:port:rtsp",
                                          json_object_get_int_member(port_obj, "rtsp"));
    }
}

static void network_message_handler(GObject *obj, IpcamMessage *msg, gboolean timeout)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(obj);
	g_assert(IPCAM_IS_IONVIF(ionvif));

	if (!timeout && msg) {
		JsonNode *body;
		g_object_get(msg, "body", &body, NULL);
		if (body)
			ipcam_ionvif_update_network_setting(ionvif, body);
	}
}


void ipcam_ionvif_update_datetime_setting(IpcamIOnvif *ionvif, JsonNode *body)
{
    JsonObject *items_obj = json_object_get_object_member(json_node_get_object(body), "items");

    if (json_object_has_member(items_obj, "timezone")) {
		ipcam_ionvif_set_string_property(ionvif, "datetime:timezone",
										 json_object_get_string_member(items_obj, "timezone"));
	}
    if (json_object_has_member(items_obj, "use_ntp")) {
		ipcam_ionvif_set_int_property(ionvif, "datetime:use_ntp",
									  json_object_get_boolean_member(items_obj, "use_ntp"));
	}
    if (json_object_has_member(items_obj, "ntp_server")) {
		ipcam_ionvif_set_string_property(ionvif, "datetime:ntp_server",
										 json_object_get_string_member(items_obj, "ntp_server"));
	}
}

void ipcam_ionvif_update_video_setting(IpcamIOnvif *ionvif, JsonNode *body)
{
    JsonObject *items_obj = json_object_get_object_member(json_node_get_object(body), "items");

    if (json_object_has_member(items_obj, "profile")) {
		ipcam_ionvif_set_string_property(ionvif, "video:profile",
										 json_object_get_string_member(items_obj, "profile"));
	}
    if (json_object_has_member(items_obj, "flip")) {
		ipcam_ionvif_set_int_property(ionvif, "video:flip",
									  json_object_get_boolean_member(items_obj, "flip"));
	}
    if (json_object_has_member(items_obj, "mirror")) {
		ipcam_ionvif_set_int_property(ionvif, "video:mirror",
		                              json_object_get_boolean_member(items_obj, "mirror"));
	}
	if (json_object_has_member(items_obj, "main_profile")) {
		JsonObject *p_obj = json_object_get_object_member(items_obj, "main_profile");
		if (json_object_has_member(p_obj, "frame_rate")) {
			ipcam_ionvif_set_int_property(ionvif, "video:main_profile:frame_rate",
			                              json_object_get_int_member(p_obj, "frame_rate"));
		}
		if (json_object_has_member(p_obj, "bit_rate")) {
			ipcam_ionvif_set_string_property(ionvif, "video:main_profile:bit_rate",
			                              json_object_get_string_member(p_obj, "bit_rate"));
		}
		if (json_object_has_member(p_obj, "bit_rate_value")) {
			ipcam_ionvif_set_int_property(ionvif, "video:main_profile:bit_rate_value",
			                              json_object_get_int_member(p_obj, "bit_rate_value"));
		}
		if (json_object_has_member(p_obj, "resolution")) {
			ipcam_ionvif_set_string_property(ionvif, "video:main_profile:resolution",
			                              json_object_get_string_member(p_obj, "resolution"));
		}
		if (json_object_has_member(p_obj, "stream_path")) {
			ipcam_ionvif_set_string_property(ionvif, "video:main_profile:stream_path",
			                              json_object_get_string_member(p_obj, "stream_path"));
		}
	}
	if (json_object_has_member(items_obj, "sub_profile")) {
		JsonObject *p_obj = json_object_get_object_member(items_obj, "sub_profile");
		if (json_object_has_member(p_obj, "frame_rate")) {
			ipcam_ionvif_set_int_property(ionvif, "video:sub_profile:frame_rate",
			                              json_object_get_int_member(p_obj, "frame_rate"));
		}
		if (json_object_has_member(p_obj, "bit_rate")) {
			ipcam_ionvif_set_string_property(ionvif, "video:sub_profile:bit_rate",
			                              json_object_get_string_member(p_obj, "bit_rate"));
		}
		if (json_object_has_member(p_obj, "bit_rate_value")) {
			ipcam_ionvif_set_int_property(ionvif, "video:sub_profile:bit_rate_value",
			                              json_object_get_int_member(p_obj, "bit_rate_value"));
		}
		if (json_object_has_member(p_obj, "resolution")) {
			ipcam_ionvif_set_string_property(ionvif, "video:sub_profile:resolution",
			                              json_object_get_string_member(p_obj, "resolution"));
		}
		if (json_object_has_member(p_obj, "stream_path")) {
			ipcam_ionvif_set_string_property(ionvif, "video:sub_profile:stream_path",
			                              json_object_get_string_member(p_obj, "stream_path"));
		}
	}
}

static void datetime_message_handler(GObject *obj, IpcamMessage *msg, gboolean timeout)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(obj);
	g_assert(IPCAM_IS_IONVIF(ionvif));

	if (!timeout && msg) {
		JsonNode *body;
		g_object_get(msg, "body", &body, NULL);
		if (body)
			ipcam_ionvif_update_datetime_setting(ionvif, body);
	}
}

static void video_message_handler(GObject *obj, IpcamMessage *msg, gboolean timeout)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(obj);
	g_assert(IPCAM_IS_IONVIF(ionvif));

	if (!timeout && msg) {
		JsonNode *body;
		g_object_get(msg, "body", &body, NULL);
		if (body)
			ipcam_ionvif_update_video_setting(ionvif, body);
	}
}

void ipcam_ionvif_update_users(IpcamIOnvif *ionvif, JsonNode *body)
{
	IpcamIOnvifPrivate *priv = ionvif->priv;
	JsonArray *items = json_object_get_array_member(json_node_get_object(body), "items");
	int i;

	g_mutex_lock(&priv->mutex);
	for (i = 0; i < json_array_get_length(items); i++) {
		JsonObject *obj = json_array_get_object_element(items, i);
		gchar *username = json_object_get_string_member(obj, "username");
		gchar *password = json_object_get_string_member(obj, "password");

		if (username && password) {
			g_hash_table_insert(priv->cached_users_hash, 
								 g_strdup(username),
								 g_strdup(password));
		}
	}
	g_mutex_unlock(&priv->mutex);
}

void ipcam_ionvif_delete_users(IpcamIOnvif *ionvif, JsonNode *body)
{
    IpcamIOnvifPrivate *priv = ionvif->priv;
    JsonArray *item_arr;
    JsonObject *obj;
    int i;

    item_arr = json_object_get_array_member(json_node_get_object(body), "items");

	g_mutex_lock(&priv->mutex);
    for (i = 0; i < json_array_get_length (item_arr); i++) {
        char *username;

        obj = json_array_get_object_element (item_arr, i);
        username = json_object_get_string_member(obj, "username");

		if (username)
			g_hash_table_remove(priv->cached_users_hash, username);
	}
	g_mutex_unlock(&priv->mutex);
}

static void users_message_handler(GObject *obj, IpcamMessage *msg, gboolean timeout)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(obj);
	g_assert(IPCAM_IS_IONVIF(ionvif));
	IpcamIOnvifPrivate *priv = ionvif->priv;

	if (!timeout && msg) {
		JsonNode *body = NULL;
		g_object_get(msg, "body", &body, NULL);
		if (body) {
			ipcam_ionvif_update_users(ionvif, body);
		}
	}
}


static void ipcam_ionvif_before_start(IpcamBaseService *base_service)
{
	IpcamIOnvif *ionvif = IPCAM_IONVIF(base_service);
	IpcamIOnvifPrivate *priv = ionvif->priv;
	JsonBuilder *builder;
	const gchar *token = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "token");
	IpcamRequestMessage *req_msg;

	priv->service_thread = g_thread_new("onvif-server",
	                                    onvif_server_thread_func,
	                                    ionvif);
	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif), "set_base_info", IPCAM_TYPE_IONVIF_EVENT_HANDLER);
	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif), "set_datetime", IPCAM_TYPE_IONVIF_EVENT_HANDLER);
	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif), "set_network", IPCAM_TYPE_IONVIF_EVENT_HANDLER);
	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif), "set_video", IPCAM_TYPE_IONVIF_EVENT_HANDLER);
	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif), "add_users", IPCAM_TYPE_IONVIF_EVENT_HANDLER);
	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif), "set_users", IPCAM_TYPE_IONVIF_EVENT_HANDLER);
	ipcam_base_app_register_notice_handler(IPCAM_BASE_APP(ionvif), "del_users", IPCAM_TYPE_IONVIF_EVENT_HANDLER);

	/* Request the Base Information */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "device_name");
	json_builder_add_string_value(builder, "comment");
	json_builder_add_string_value(builder, "location");
	json_builder_add_string_value(builder, "hardware");
	json_builder_add_string_value(builder, "firmware");
	json_builder_add_string_value(builder, "manufacturer");
	json_builder_add_string_value(builder, "model");
	json_builder_add_string_value(builder, "serial");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_base_info",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token,
	                            base_info_message_handler, 60);
	g_object_unref(req_msg);
	g_object_unref(builder);

	/* Request the Network setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "method");
	json_builder_add_string_value(builder, "address");
	json_builder_add_string_value(builder, "pppoe");
	json_builder_add_string_value(builder, "port");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_network",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token,
	                            network_message_handler, 60);
	g_object_unref(req_msg);
	g_object_unref(builder);

	/* Request the Datetime setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "timezone");
	json_builder_add_string_value(builder, "use_ntp");
	json_builder_add_string_value(builder, "ntp_server");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_datetime",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token,
	                            datetime_message_handler, 60);
	g_object_unref(req_msg);
	g_object_unref(builder);

	/* Request the Video setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "profile");
	json_builder_add_string_value(builder, "flip");
	json_builder_add_string_value(builder, "mirror");
	json_builder_add_string_value(builder, "main_profile");
	json_builder_add_string_value(builder, "sub_profile");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_video",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token,
	                            video_message_handler, 60);
	g_object_unref(req_msg);
	g_object_unref(builder);

	/* Request the Video setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "password");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_users",
	                       "body", json_builder_get_root(builder),
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif), IPCAM_MESSAGE(req_msg),
	                            "iconfig", token,
	                            users_message_handler, 60);
	g_object_unref(req_msg);
	g_object_unref(builder);
}

static void ipcam_ionvif_in_loop(IpcamBaseService *base_service)
{
}

gboolean ipcam_ionvif_is_terminating(IpcamIOnvif *ionvif)
{
	return ionvif->priv->terminating;
}

const gpointer ipcam_ionvif_get_property(IpcamIOnvif *ionvif, const gchar *key)
{
	IpcamIOnvifPrivate *priv = ionvif->priv;
	gpointer ret;

	g_mutex_lock(&priv->mutex);
	ret = g_hash_table_lookup(priv->cached_property_hash, key);
	g_mutex_unlock(&priv->mutex);

	return ret;
}

void ipcam_ionvif_set_property(IpcamIOnvif *ionvif, gchar *key, gpointer value)
{
	IpcamIOnvifPrivate *priv = ionvif->priv;

	g_mutex_lock(&priv->mutex);
	g_hash_table_insert(priv->cached_property_hash, (gpointer)key, value);
	g_mutex_unlock(&priv->mutex);
}

gboolean ionvif_get_user_password(IpcamIOnvif *ionvif, const gchar *username, gchar **password)
{
	IpcamIOnvifPrivate *priv = ionvif->priv;
	gboolean ret = FALSE;

	g_mutex_lock(&priv->mutex);
	ret = g_hash_table_lookup_extended(priv->cached_users_hash, username, NULL, password);
	g_mutex_unlock(&priv->mutex);

	return ret;
}

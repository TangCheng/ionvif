#include "soapH.h"
#include <assert.h>
#include <json-glib/json-glib.h>
#include "onvif_impl.h"
#include "onvif-authentication.h"
#include "request_message.h"
#include "ionvif.h"

int __tds__GetUsers(struct soap *soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse)
{
	IpcamIOnvif *ionvif = *(IpcamIOnvif **)soap->user;
	const gchar *token;
	IpcamRequestMessage *req_msg;
	JsonNode *req_body;
	IpcamMessage *resp_msg;
	JsonBuilder *builder;

	ACCESS_CONTROL;

	token = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "token");
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "password");
	json_builder_add_string_value(builder, "privilege");
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	req_body = json_builder_get_root(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "get_users",
	                       "body", req_body,
	                       NULL);
	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif),
	                            IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, NULL, 10);

	if (ipcam_base_app_wait_response(IPCAM_BASE_APP(ionvif),
	                                 ipcam_request_message_get_id(req_msg),
	                                 5000, &resp_msg))
	{
		JsonNode *resp_body;
		JsonArray *resp_array;
		int i;

		g_object_get(G_OBJECT(resp_msg), "body", &resp_body, NULL);
		resp_array = json_object_get_array_member(json_node_get_object(resp_body), "items");

		tds__GetUsersResponse->__sizeUser = json_array_get_length(resp_array);

		SOAP_CALLOC(soap, tds__GetUsersResponse->User, tds__GetUsersResponse->__sizeUser);

		for (i = 0; i < json_array_get_length(resp_array); i++) {
			JsonObject *obj = json_array_get_object_element(resp_array, i);
			const gchar *username = json_object_get_string_member(obj, "username");
			const gchar *password = json_object_get_string_member(obj, "password");
			gint privilege = json_object_get_int_member(obj, "privilege");
			SOAP_SET_STRING_FIELD(soap, tds__GetUsersResponse->User[i].Username, username);
			SOAP_SET_STRING_FIELD(soap, tds__GetUsersResponse->User[i].Password, password);
			SOAP_SET_VALUE_FIELD(soap, tds__GetUsersResponse->User[i].UserLevel, privilege);
		}

		g_object_unref(resp_msg);
	}

	g_object_unref(req_msg);
	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__CreateUsers(struct soap *soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse)
{
	IpcamIOnvif *ionvif = *(IpcamIOnvif **)soap->user;
	const gchar *token;
	IpcamRequestMessage *req_msg;
	JsonNode *req_body;
	IpcamMessage *resp_msg;
	JsonBuilder *builder;
	int i;

	ACCESS_CONTROL;

	token = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "token");
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	for (i = 0; i < tds__CreateUsers->__sizeUser; i++) {
		json_builder_begin_object(builder);
		json_builder_set_member_name(builder, "username");
		json_builder_add_string_value(builder, tds__CreateUsers->User[i].Username);
		json_builder_set_member_name(builder, "password");
		json_builder_add_string_value(builder, tds__CreateUsers->User[i].Password);
		json_builder_set_member_name(builder, "privilege");
		json_builder_add_int_value(builder, tds__CreateUsers->User[i].UserLevel);
		json_builder_end_object(builder);
	}
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	req_body = json_builder_get_root(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "add_users",
	                       "body", req_body,
	                       NULL);

	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif),
	                            IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, NULL, 10);

	if (ipcam_base_app_wait_response(IPCAM_BASE_APP(ionvif),
	                                 ipcam_request_message_get_id(req_msg),
	                                 5000, &resp_msg))
	{
		g_object_unref(resp_msg);
	}

	g_object_unref(req_msg);
	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__DeleteUsers(struct soap *soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse)
{	
	IpcamIOnvif *ionvif = *(IpcamIOnvif **)soap->user;
	const gchar *token;
	IpcamRequestMessage *req_msg;
	JsonNode *req_body;
	IpcamMessage *resp_msg;
	JsonBuilder *builder;
	int i;

	ACCESS_CONTROL;

	token = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "token");
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	for (i = 0; i < tds__DeleteUsers->__sizeUsername; i++) {
		json_builder_begin_object(builder);
		json_builder_set_member_name(builder, "username");
		json_builder_add_string_value(builder, tds__DeleteUsers->Username[i]);
		json_builder_end_object(builder);
	}
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	req_body = json_builder_get_root(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "del_users",
	                       "body", req_body,
	                       NULL);

	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif),
	                            IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, NULL, 10);

	if (ipcam_base_app_wait_response(IPCAM_BASE_APP(ionvif),
	                                 ipcam_request_message_get_id(req_msg),
	                                 5000, &resp_msg))
	{
		g_object_unref(resp_msg);
	}

	g_object_unref(req_msg);
	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__SetUser(struct soap *soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse)
{	
	IpcamIOnvif *ionvif = *(IpcamIOnvif **)soap->user;
	const gchar *token;
	IpcamRequestMessage *req_msg;
	JsonNode *req_body;
	IpcamMessage *resp_msg;
	JsonBuilder *builder;
	int i;

	ACCESS_CONTROL;

	token = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "token");
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	for (i = 0; i < tds__SetUser->__sizeUser; i++) {
		json_builder_begin_object(builder);
		json_builder_set_member_name(builder, "username");
		json_builder_add_string_value(builder, tds__SetUser->User[i].Username);
		json_builder_set_member_name(builder, "password");
		json_builder_add_string_value(builder, tds__SetUser->User[i].Password);
		json_builder_set_member_name(builder, "privilege");
		json_builder_add_int_value(builder, tds__SetUser->User[i].UserLevel);
		json_builder_end_object(builder);
	}
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	req_body = json_builder_get_root(builder);
	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", "set_users",
	                       "body", req_body,
	                       NULL);

	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif),
	                            IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, NULL, 10);

	if (ipcam_base_app_wait_response(IPCAM_BASE_APP(ionvif),
	                                 ipcam_request_message_get_id(req_msg),
	                                 5000, &resp_msg))
	{
		g_object_unref(resp_msg);
	}

	g_object_unref(req_msg);
	g_object_unref(builder);

	return SOAP_OK;
}

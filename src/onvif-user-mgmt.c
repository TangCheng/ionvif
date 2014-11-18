#include "soapH.h"
#include <assert.h>
#include <json-glib/json-glib.h>
#include "onvif_impl.h"
#include "onvif-auth.h"
#include "request_message.h"
#include "ionvif.h"

static const char *UserLevelToRole(enum tt__UserLevel user_level)
{
	switch (user_level) {
		case tt__UserLevel__Administrator:
			return "administrator";
		case tt__UserLevel__Operator:
			return "operator";
		case tt__UserLevel__User:
			return "user";
		case tt__UserLevel__Anonymous:
			return "anonymous";
		case tt__UserLevel__Extended:
		default:
			return "extended";
	}
	return "anonymous";
}

static enum tt__UserLevel RoleToUserLevel(const char *role)
{
	if (g_strcmp0(role, "administrator") == 0)
		return tt__UserLevel__Administrator;
	if (g_strcmp0(role, "operator") == 0)
		return tt__UserLevel__Operator;
	if (g_strcmp0(role, "user") == 0)
		return tt__UserLevel__User;
	if (g_strcmp0(role, "anonymous") == 0)
		return tt__UserLevel__Anonymous;

	return tt__UserLevel__Extended;
}

int __tds__GetUsers(struct soap *soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonNode *response;
	JsonBuilder *builder;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "password");
	json_builder_add_string_value(builder, "role");
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "get_users", json_builder_get_root(builder), &response)) {
		JsonArray *items;
		int i;

		items = json_object_get_array_member(json_node_get_object(response), "items");

		tds__GetUsersResponse->__sizeUser = json_array_get_length(items);

		SOAP_CALLOC(soap, tds__GetUsersResponse->User, tds__GetUsersResponse->__sizeUser);

		for (i = 0; i < json_array_get_length(items); i++) {
			JsonObject *obj = json_array_get_object_element(items, i);
			const gchar *username = json_object_get_string_member(obj, "username");
			const gchar *password = json_object_get_string_member(obj, "password");
			const gchar *role = json_object_get_string_member(obj, "role");
			SOAP_SET_STRING_FIELD(soap, tds__GetUsersResponse->User[i].Username, username);
			SOAP_SET_STRING_FIELD(soap, tds__GetUsersResponse->User[i].Password, password);
			SOAP_SET_VALUE_FIELD(soap, tds__GetUsersResponse->User[i].UserLevel, 
			                     RoleToUserLevel(role));
		}

		json_node_free(response);
	}
	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__CreateUsers(struct soap *soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonNode *response;
	JsonBuilder *builder;
	int i;

	ACCESS_CONTROL;

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
		json_builder_set_member_name(builder, "role");
		json_builder_add_string_value(builder, 
		                              UserLevelToRole(tds__CreateUsers->User[i].UserLevel));
		json_builder_end_object(builder);
	}
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "add_users", json_builder_get_root(builder), &response))
		json_node_free(response);

	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__DeleteUsers(struct soap *soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse)
{	
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonNode *response;
	JsonBuilder *builder;
	int i;

	ACCESS_CONTROL;

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

	if (onvif_invocate_action(ionvif, "del_users", json_builder_get_root(builder), &response))
		json_node_free(response);

	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__SetUser(struct soap *soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse)
{	
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonNode *response;
	JsonBuilder *builder;
	int i;

	ACCESS_CONTROL;

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
		json_builder_set_member_name(builder, "role");
		json_builder_add_string_value(builder, UserLevelToRole(tds__SetUser->User[i].UserLevel));
		json_builder_end_object(builder);
	}
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "set_users", json_builder_get_root(builder), &response))
		json_node_free(response);

	g_object_unref(builder);

	return SOAP_OK;
}

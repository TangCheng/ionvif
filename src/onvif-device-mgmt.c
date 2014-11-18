#include "soapH.h"
#include <assert.h>
#include <json-glib/json-glib.h>
#include "request_message.h"
#include "onvif_impl.h"
#include "onvif-auth.h"


const static char *__fixed_onvif_scopes[] = {
	"onvif://www.onvif.org/name/IPCAM",
	"onvif://www.onvif.org/location/China"
};


/** Auto-test server operation __tds__GetScopes */
int __tds__GetScopes(struct soap *soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	int nr_var_scopes = 0;
	char *var_scopes[2] = { NULL, NULL };
	int i;

	ACCESS_CONTROL;

	const gchar *device_name = ipcam_ionvif_get_string_property(ionvif, "base_info:device_name");
	const gchar *location = ipcam_ionvif_get_string_property(ionvif, "base_info:location");

    if (device_name) {
        gchar *str;
        if (asprintf(&str, "odm:name:%s", device_name) > 0)
            var_scopes[nr_var_scopes++] = str;
    }
    if (location) {
        gchar *str;
        if (asprintf(&str, "odm:location:%s", location) > 0)
            var_scopes[nr_var_scopes++] = str;
    }

	tds__GetScopesResponse->__sizeScopes = ARRAY_SIZE(__fixed_onvif_scopes) + nr_var_scopes;

	SOAP_CALLOC(soap, tds__GetScopesResponse->Scopes, tds__GetScopesResponse->__sizeScopes);

	for (i = 0; i < ARRAY_SIZE(__fixed_onvif_scopes); i++) {
		SOAP_SET_VALUE_FIELD (soap, tds__GetScopesResponse->Scopes[i].ScopeDef, tt__ScopeDefinition__Fixed);
		SOAP_SET_STRING_FIELD(soap, tds__GetScopesResponse->Scopes[i].ScopeItem, __fixed_onvif_scopes[i]);
	}

	for (i = 0; i < nr_var_scopes; i++) {
		int off = ARRAY_SIZE(__fixed_onvif_scopes);
		SOAP_SET_VALUE_FIELD (soap, tds__GetScopesResponse->Scopes[off + i].ScopeDef, tt__ScopeDefinition__Fixed);
		SOAP_SET_STRING_FIELD(soap, tds__GetScopesResponse->Scopes[off + i].ScopeItem, var_scopes[i]);
		free(var_scopes[i]);
	}

	return SOAP_OK;
}

/** Auto-test server operation __tds__SetScopes */
int __tds__SetScopes(struct soap *soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;
	JsonNode *response = NULL;
	int i;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	for (i = 0; i < tds__SetScopes->__sizeScopes; i++) {
		int j;
		struct {
			const char *name;
			const char *odm_var;
		} tbl[] = {
			{ "device_name", "odm:name:" },
			{ "location", "odm:location:" }
		};

		for (j = 0; j < ARRAY_SIZE(tbl); j++) {
			int len = strlen(tbl[j].odm_var);
			if (strncmp(tds__SetScopes->Scopes[i], tbl[j].odm_var, len) == 0) {
				json_builder_set_member_name(builder, tbl[j].name);
				json_builder_add_string_value(builder, &tds__SetScopes->Scopes[i][len]);
			}
		}
	}
	json_builder_end_object(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "set_base_info", json_builder_get_root(builder), &response)) {
		json_node_free(response);
	}

	g_object_unref(builder);

	return SOAP_OK;
}


//******************************************************************************************************************************************************************
int __tds__GetServices(struct soap* soap,
		struct _tds__GetServices *tds__GetServices,
		struct _tds__GetServicesResponse *tds__GetServicesResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
    gchar *service_url = NULL;

	ACCESS_CONTROL;

	tds__GetServicesResponse->__sizeService = 1;

	SOAP_CALLOC_1(soap, tds__GetServicesResponse->Service);
	SOAP_CALLOC_1(soap, tds__GetServicesResponse->Service[0].Version);

    asprintf(&service_url, "http://%s/onvif/ver10/device/wsdl/devicemgmt.wsdl",
             ipcam_ionvif_get_string_property(ionvif, "network:address:ipaddr"));
	SOAP_SET_STRING_FIELD(soap, tds__GetServicesResponse->Service[0].XAddr, service_url);
    free(service_url);
#define SERVICE_NAMESPACE "http://www.onvif.org/ver10/events/wsdl"
	SOAP_SET_STRING_FIELD(soap, tds__GetServicesResponse->Service[0].Namespace, SERVICE_NAMESPACE);
	tds__GetServicesResponse->Service[0].Version->Major = 1;
	tds__GetServicesResponse->Service[0].Version->Minor = 0;

	return SOAP_OK;
}

int __tds__GetDeviceInformation(struct soap* soap,
		struct _tds__GetDeviceInformation *tds__GetDeviceInformation,
		struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse)
{
	struct _tds__GetDeviceInformationResponse *Response = tds__GetDeviceInformationResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
    const gchar *hardware_id = ipcam_ionvif_get_string_property(ionvif, "base_info:hardware");
    const gchar *firmware_ver = ipcam_ionvif_get_string_property(ionvif, "base_info:firmware");
    const gchar *manufacturer = ipcam_ionvif_get_string_property(ionvif, "base_info:manufacturer");
    const gchar *model = ipcam_ionvif_get_string_property(ionvif, "base_info:model");
    const gchar *serial = ipcam_ionvif_get_string_property(ionvif, "base_info:serial");

	ACCESS_CONTROL;

	SOAP_SET_STRING_FIELD(soap, Response->FirmwareVersion, firmware_ver);
	SOAP_SET_STRING_FIELD(soap, Response->HardwareId, hardware_id);
	SOAP_SET_STRING_FIELD(soap, Response->Manufacturer, manufacturer);
	SOAP_SET_STRING_FIELD(soap, Response->Model, model);
	SOAP_SET_STRING_FIELD(soap, Response->SerialNumber, serial);

	return SOAP_OK;
}


static int save_file_data(const char* path, void const* buffer, int length) {
	FILE* pf = fopen(path, "wb");
	if (pf != NULL) {
		fwrite(buffer, length, 1, pf);
		fclose(pf);
		return 1;
	} else {
		printf("open file failed %s\n", path);
	}
	return 0;
}

int __tds__UpgradeSystemFirmware(struct soap* soap,
		struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware,
		struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse)
{
	ACCESS_CONTROL;

	if (tds__UpgradeSystemFirmware->Firmware == NULL) {
		printf("Firmware is NULL\n");
	} else {
		printf("mimeType:%s\n", tds__UpgradeSystemFirmware->Firmware->xmime__contentType);
		//printf("", ds__UpgradeSystemFirmware->

		printf("id:%s\n", tds__UpgradeSystemFirmware->Firmware->xop__Include.id);
		printf("options:%s\n", tds__UpgradeSystemFirmware->Firmware->xop__Include.options);
		printf("type:%s\n", tds__UpgradeSystemFirmware->Firmware->xop__Include.type);
		printf("size:%d\n", tds__UpgradeSystemFirmware->Firmware->xop__Include.__size);
		if (tds__UpgradeSystemFirmware->Firmware->xop__Include.__size > 0) {
			int ret = save_file_data("./firmware.bin",
					tds__UpgradeSystemFirmware->Firmware->xop__Include.__ptr,
					tds__UpgradeSystemFirmware->Firmware->xop__Include.__size);
			if (ret) {
				printf("*************save firmware OK\n");
			}
		}
	}

	return SOAP_OK;
}


int __tds__GetWsdlUrl(struct soap* soap,
		struct _tds__GetWsdlUrl *tds__GetWsdlUrl,
		struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
    char *wsdl_url = NULL;

    ACCESS_CONTROL;

    asprintf(&wsdl_url, "http://%s/onvif/ver10/device/wsdl/devicemgmt.wsdl",
             ipcam_ionvif_get_string_property(ionvif, "network:address:ipaddr"));
	SOAP_SET_STRING_FIELD(soap,  tds__GetWsdlUrlResponse->WsdlUrl, wsdl_url);
    free(wsdl_url);

	return SOAP_OK;
}


int __tds__GetCapabilities(struct soap* soap,
		struct _tds__GetCapabilities *tds__GetCapabilities,
		struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	struct _tds__GetCapabilitiesResponse *Response = tds__GetCapabilitiesResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;

	DBG_LINE;

	SOAP_CALLOC_1(soap, Response->Capabilities);
	SOAP_CALLOC_1(soap, Response->Capabilities->Imaging);
	SOAP_CALLOC_1(soap, Response->Capabilities->Media);
	SOAP_CALLOC_1(soap, Response->Capabilities->Media->StreamingCapabilities);
	SOAP_CALLOC_1(soap, Response->Capabilities->Media->StreamingCapabilities->RTPMulticast);
	SOAP_CALLOC_1(soap, Response->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP);
	SOAP_CALLOC_1(soap, Response->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP);
	SOAP_CALLOC_1(soap, Response->Capabilities->Extension);
	SOAP_CALLOC_1(soap, Response->Capabilities->Extension->DeviceIO);
	SOAP_CALLOC_1(soap, Response->Capabilities->Device);
	SOAP_CALLOC_1(soap, Response->Capabilities->Device->System);
	SOAP_CALLOC_1(soap, Response->Capabilities->Device->System->SupportedVersions);


	SOAP_SET_STRING_FIELD(soap, Response->Capabilities->Imaging->XAddr, ipcam_ionvif_get_string_property(ionvif, "network:address:ipaddr"));
	SOAP_SET_STRING_FIELD(soap, Response->Capabilities->Media->XAddr, ipcam_ionvif_get_string_property(ionvif, "network:address:ipaddr"));
	SOAP_SET_VALUE_FIELD(soap, Response->Capabilities->Media->StreamingCapabilities->RTPMulticast[0], xsd__boolean__false_);
	SOAP_SET_VALUE_FIELD(soap, Response->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP[0], xsd__boolean__true_);
	SOAP_SET_VALUE_FIELD(soap, Response->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP[0], xsd__boolean__true_);
	SOAP_SET_VALUE_FIELD(soap, Response->Capabilities->Extension->DeviceIO->VideoSources, TRUE);
	SOAP_SET_VALUE_FIELD(soap, Response->Capabilities->Device->System->FirmwareUpgrade, xsd__boolean__true_);
	SOAP_SET_VALUE_FIELD(soap, Response->Capabilities->Device->System->SupportedVersions->Major, 2);
	SOAP_SET_VALUE_FIELD(soap, Response->Capabilities->Device->System->SupportedVersions->Minor, 0);

	return SOAP_OK;
}



int __tds__SetSystemDateAndTime(struct soap *soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse)
{
    struct _tds__SetSystemDateAndTime *Request = tds__SetSystemDateAndTime;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;
	char *str_datetime = NULL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	// TimeZone
    if (Request->TimeZone && Request->TimeZone->TZ) {
        json_builder_set_member_name(builder, "timezone");
        json_builder_add_string_value(builder, Request->TimeZone->TZ);
    }
	//User NTP
	json_builder_set_member_name(builder, "use_ntp");
	json_builder_add_int_value(builder, Request->DateTimeType);
	// DateTime
    if (Request->UTCDateTime) {
        json_builder_set_member_name(builder, "datetime");
        asprintf(&str_datetime, "%04d-%02d-%02d %02d:%02d:%02d",
                 Request->UTCDateTime->Date->Year,
                 Request->UTCDateTime->Date->Month,
                 Request->UTCDateTime->Date->Day,
                 Request->UTCDateTime->Time->Hour,
                 Request->UTCDateTime->Time->Minute,
                 Request->UTCDateTime->Time->Second);
        json_builder_add_string_value(builder, str_datetime);
        free(str_datetime);
    }

	json_builder_end_object(builder);

	onvif_invocate_action(ionvif, "set_datetime", json_builder_get_root(builder), NULL);

	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__GetSystemDateAndTime(struct soap *soap, struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse)
{
	struct _tds__GetSystemDateAndTimeResponse *Response = tds__GetSystemDateAndTimeResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;
	JsonNode *response = NULL;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "datetime");
	json_builder_end_array(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "get_datetime", json_builder_get_root(builder), &response)) {
		JsonObject *items = json_object_get_object_member(json_node_get_object(response), "items");

		SOAP_CALLOC_1(soap, Response->SystemDateAndTime);

        const char *tz = ipcam_ionvif_get_string_property(ionvif, "datetime:timezone");
        SOAP_CALLOC_1(soap, Response->SystemDateAndTime->TimeZone);
        SOAP_SET_STRING_FIELD(soap, Response->SystemDateAndTime->TimeZone->TZ, tz);

        int use_ntp = !!ipcam_ionvif_get_int_property(ionvif, "datetime:use_ntp");
        SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->DateTimeType, use_ntp);

        if (json_object_has_member(items, "datetime")) {
			int year, month, day, hour, minute, second;
			const char *datetime = json_object_get_string_member(items, "datetime");

			if (sscanf(datetime, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6) {
				SOAP_CALLOC_1(soap, Response->SystemDateAndTime->UTCDateTime);
				SOAP_CALLOC_1(soap, Response->SystemDateAndTime->UTCDateTime->Date);
				SOAP_CALLOC_1(soap, Response->SystemDateAndTime->UTCDateTime->Time);

				SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->UTCDateTime->Date->Year, year);
				SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->UTCDateTime->Date->Month, month);
				SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->UTCDateTime->Date->Day, day);
				SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->UTCDateTime->Time->Hour, hour);
				SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->UTCDateTime->Time->Minute, minute);
				SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->UTCDateTime->Time->Second, second);
				SOAP_SET_VALUE_FIELD(soap, Response->SystemDateAndTime->DaylightSavings, xsd__boolean__false_);
			}
		}
		json_node_free(response);
	}
	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__SetSystemFactoryDefault(struct soap *soap,
		struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault,
		struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse)
{
	return SOAP_OK;
}


int __tds__SystemReboot(struct soap *soap,
		struct _tds__SystemReboot *tds__SystemReboot,
		struct _tds__SystemRebootResponse *tds__SystemRebootResponse)
{
	return SOAP_OK;
}


int __tds__GetAccessPolicy(struct soap *soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse)
{
	/* Return incomplete response with default data values */
	DBG_LINE
	return SOAP_OK;
}


int __tds__SetAccessPolicy(struct soap *soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse)
{
	DBG_LINE
	return SOAP_OK;
}


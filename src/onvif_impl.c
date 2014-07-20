#include "soapH.h"
#include <assert.h>
#include <json-glib/json-glib.h>
#include "request_message.h"
#include "onvif_impl.h"
#include "ionvif.h"

gboolean onvif_invocate_action(IpcamIOnvif *ionvif, const char *action,
                               JsonNode *request, JsonNode **response)
{
	const gchar *token;
	IpcamRequestMessage *req_msg;
	IpcamMessage *resp_msg;
	gboolean ret = FALSE;

	token = ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), "token");

	req_msg = g_object_new(IPCAM_REQUEST_MESSAGE_TYPE,
	                       "action", action,
	                       "body", request,
	                       NULL);

	ipcam_base_app_send_message(IPCAM_BASE_APP(ionvif),
	                            IPCAM_MESSAGE(req_msg),
	                            "iconfig", token, NULL, 10);

	ret = ipcam_base_app_wait_response(IPCAM_BASE_APP(ionvif),
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//an device implement can overwrite these functions.
//device management functions

#define IPV4_ADDRESS "192.168.1.4"
#define WSDL_URL "http://192.168.1.4/onvif/ver10/device/wsdl/devicemgmt.wsdl"
#define SERVICE_URL "http://192.168.1.4/onvif/services"
#define SERVICE_NAMESPACE "http://www.onvif.org/ver10/events/wsdl"


const char * onvif_dm_get_ipv4_address(struct soap* soap)
{
	return IPV4_ADDRESS;
}

const char * onvif_dm_get_wsdl_url(struct soap* soap) {
	return WSDL_URL;
}

const char * onvif_dm_get_service_url(struct soap* soap) {
	return SERVICE_URL;
}

const char * onvif_dm_get_service_namespace(struct soap* soap) {
	return SERVICE_NAMESPACE;
}

void onvif_dm_get_service_version(struct soap* soap, int* major, int * minor) {
	*major = 1;
	*minor = 3;
}

const char* onvif_dm_get_manufacturer(struct soap* soap) {
	return "IPCAM";
}
const char* onvif_dm_get_model(struct soap* soap) {
	return "102";
}
const char* onvif_dm_get_firmware_version(struct soap* soap) {
	return "V1.0.0";
}
const char* onvif_dm_get_serial_number(struct soap* soap) {
	return "333-133-333";
}
const char* onvif_dm_get_hardware_id(struct soap* soap) {
	return "Hi3518";
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//media

static ONVIF_VIDEO_SOURCE __onvif_video_source[] = {
	[0] = {
		.token = "VideoSourceToken",
		.Framerate = 25,
		.Resolution = { .Width = 1280, .Height = 720 },
		.Imaging = {
			.Brightness = 128.0,
			.ColorSaturation = 128.0,
			.Contrast = 128.0,
			.IrCutFilter = 0.0,
			.Sharpness = 128.0,
			.BacklightCompensation = { .Mode = 0, .Level = 20.0 },
			.WideDynamicRange = { .Mode = 0, .Level = 0.0 },
			.WhiteBalance = { .Mode = 0, .CrGain = 0.0, .CbGain = 0.0 }
		}
	}
};

static ONVIF_VIDEO_SOURCES __onvif_video_sources = {
	.numVideoSources = ARRAY_SIZE(__onvif_video_source),
	.VideoSources = __onvif_video_source
};

static ONVIF_VIDEO_CONFIG __onvif_video_config[] = {
	[0] = {
		.Name = "VideoSouceConfig",
		.token = "VideoSourceConfigToken",
		.UseCount = 2,
		.SourceToken = "VideoSourceToken",
		.Bounds = { .x = 1, .y = 1, .width = 1280, .height = 720 }
	}
};

static ONVIF_VIDEO_ENCODER_CONFIG __onvif_video_encoder_config[] = {
	[0] = {
		.Name = "MainVideoEncoderConfig",
		.token = "MainVideoEncoderConfigToken",
		.UseCount = 1,
		.Encoding = 2,
		.Resolution = { .Width = 1280, .Height = 720 },
		.Quality = 6.0,
		.RateControl = {
			.FrameRateLimit = 25,
			.EncodingInterval = 50,
			.BitrateLimit = 2048
		}
	},
	[1] = {
		.Name = "SubVideoEncoderConfig",
		.token = "SubVideoEncoderConfigToken",
		.UseCount = 1,
		.Encoding = 2,
		.Resolution = { .Width = 640, .Height = 352 },
		.Quality = 3,
		.RateControl = {
			.FrameRateLimit = 10,
			.EncodingInterval = 10,
			.BitrateLimit = 256
		}
	}
};

static ONVIF_PROFILE __onvif_profile[] = {
	[0] = {
		.Name = "MainProfile",
		.token = "MainProfileToken",
		.VideoSourceConfig = &__onvif_video_config[0],
		.AudioSourceConfig = NULL,
		.VideoEncoderConfig = &__onvif_video_encoder_config[0],
		.AudioSourceConfig = NULL
	},
	[1] = {
		.Name = "SubProfile",
		.token = "SubProfileToken",
		.VideoSourceConfig = &__onvif_video_config[0],
		.AudioSourceConfig = NULL,
		.VideoEncoderConfig = &__onvif_video_encoder_config[1],
		.AudioSourceConfig = NULL
	}
};

static ONVIF_PROFILES __onvif_profiles = {
	.numProfiles = ARRAY_SIZE(__onvif_profile),
	.Profiles = __onvif_profile
};

gboolean
onvif_media_get_video_sources(struct soap *soap,
                              ONVIF_VIDEO_SOURCES **video_sources)
{
	*video_sources = &__onvif_video_sources;
	return TRUE;
}

gboolean
onvif_media_get_profile(struct soap *soap,
                        char *token,
                        ONVIF_PROFILE **profile)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(__onvif_profile); i++) {
		if (strcmp(token, __onvif_profile[i].token) == 0) {
			*profile = &__onvif_profile[i];
			return TRUE;
		}
	}
	return FALSE;
}

gboolean
onvif_media_get_profiles(struct soap *soap,
                         ONVIF_PROFILES **profiles)
{
	*profiles = &__onvif_profiles;
	return TRUE;
}

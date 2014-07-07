#include "soapH.h"
#include <assert.h>
#include "onvif_impl.h"
#include "ionvif.h"

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x[0]))

//following functions used to simplify set xxxResponse fields
void soap_set_field_string(struct soap* soap, char ** p_field, const char* val) {
	assert(val != 0);
	*p_field = soap_malloc(soap, strlen(val) + 1);
	strcpy(*p_field, val);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//an device implement can overwrite these functions.
//device management functions

#define IPV4_ADDRESS "192.168.1.4"
#define WSDL_URL "http://192.168.1.4/onvif/ver10/device/wsdl/devicemgmt.wsdl"
#define SERVICE_URL "http://192.168.1.4/onvif/services"
#define SERVICE_NAMESPACE "http://www.onvif.org/ver10/events/wsdl"


struct _OnvifServerContext
{
	IpcamIOnvif *ionvif;
	int nr_onvif_scopes;
	char **onvif_scopes;
};


const static char *__fixed_onvif_scopes[] = {
	"onvif://www.onvif.org/name/IPCAM",
	"onvif://www.onvif.org/location/China"
};

OnvifServerContext *onvif_server_context_new(IpcamIOnvif *ionvif)
{
	OnvifServerContext *cxt = calloc(1, sizeof(OnvifServerContext));
	int i;

	if (cxt == NULL)
		return NULL;

	cxt->ionvif = ionvif;

	cxt->nr_onvif_scopes = ARRAY_SIZE(__fixed_onvif_scopes);
	cxt->onvif_scopes = malloc(sizeof(*cxt->onvif_scopes) * cxt->nr_onvif_scopes);
	for (i = 0; i < ARRAY_SIZE(__fixed_onvif_scopes); i++)
		cxt->onvif_scopes[i] = strdup(__fixed_onvif_scopes[i]);

	return cxt;
}

void onvif_server_context_destroy(OnvifServerContext *ctx)
{
	int i;

	for (i = 0; i < ctx->nr_onvif_scopes; i++)
		free(ctx->onvif_scopes[i]);
	free(ctx->onvif_scopes);
}

const char * onvif_dm_get_ipv4_address(struct soap* soap)
{
	return IPV4_ADDRESS;
}

int onvif_dm_get_scopes(struct soap *soap, char ***scopes)
{
	OnvifServerContext *cxt = (OnvifServerContext *)soap->user;

	*scopes = cxt->onvif_scopes;

	return cxt->nr_onvif_scopes;
}

void onvif_dm_set_scopes(struct soap *soap, int nr_scopes, char **scopes)
{
	OnvifServerContext *cxt = (OnvifServerContext *)soap->user;
	char **onvif_scopes;
	int nr_onvif_scopes;
	int i;

	if (nr_scopes <= 0)
		return;

	nr_onvif_scopes = cxt->nr_onvif_scopes + nr_scopes;

	/* Realloc memory */
	onvif_scopes = realloc(cxt->onvif_scopes, sizeof(*onvif_scopes) * nr_onvif_scopes);
	if (onvif_scopes) {
		for (i = 0; i < nr_scopes; i++)
			onvif_scopes[i + cxt->nr_onvif_scopes] = strdup(scopes[i]);

		cxt->onvif_scopes = onvif_scopes;
		cxt->nr_onvif_scopes = nr_onvif_scopes;
	}
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
	return "NewBee";
}
const char* onvif_dm_get_model(struct soap* soap) {
	return "102";
}
const char* onvif_dm_get_firmware_version(struct soap* soap) {
	return "1.1";
}
const char* onvif_dm_get_serial_number(struct soap* soap) {
	return "333-133-333";
}
const char* onvif_dm_get_hardware_id(struct soap* soap) {
	return "free_scale_333";
}

void onvif_dm_set_system_date_time(struct soap* soap, int is_ntp, struct tm* now) {
}

void onvif_dm_get_system_date_time(struct soap* soap, int* is_ntp, struct tm* now) {
}

void onvif_dm_set_system_factory_default(struct soap* soap, int type) {
}

void onvif_dm_reboot_system(struct soap* soap) {
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

char *
onvif_media_get_stream_uri(struct soap *soap, char *profile_token)
{
	OnvifServerContext *cxt = (OnvifServerContext *)soap->user;
	gchar *addr = ipcam_ionvif_get_server_addr(cxt->ionvif);
	guint rtsp_port = ipcam_ionvif_get_rtsp_port (cxt->ionvif);
	char *uri;
	char *key;
	char *stream_path = NULL;

	if (asprintf(&key, "onvif:profile:%s", profile_token) > 0) {
		stream_path = (char *)ipcam_base_app_get_config(IPCAM_BASE_APP(cxt->ionvif), key);
		free(key);
	}

	asprintf(&uri, "rtsp://%s:%d/%s", addr, rtsp_port, stream_path);
	char *ret = soap_strdup(soap, uri);
	free(addr);
	free(uri);

	return ret;
}

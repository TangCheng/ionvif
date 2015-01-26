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
	                                   10000, &resp_msg);
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
//media

static ONVIF_VIDEO_SOURCE __onvif_video_source[] = {
	[0] = {
		.token = "VideoSourceToken0",
		.Framerate = 25,
		.Resolution = { .Width = 1920, .Height = 1080 },
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

static ONVIF_VIDEO_SOURCE_CONFIG __onvif_video_source_config[] = {
	[0] = {
		.Name = "VideoSouceConfig0",
		.token = "VideoSourceConfigToken0",
		.UseCount = 1,
		.SourceToken = "VideoSourceToken0",
		.Bounds = { .x = 1, .y = 1, .width = 1920, .height = 1080 }
	}
};

static ONVIF_VIDEO_RESOLUTION __main_stream_resolutions_avail[] = {
    [0] = { .Width = 1600, .Height = 1200 },
    [1] = { .Width = 1920, .Height = 1080 },
    [2] = { .Width = 1280, .Height = 960  },
    [3] = { .Width = 1280, .Height = 720  },
    [4] = { .Width = 704,  .Height = 576  }
};

static ONVIF_VIDEO_RESOLUTION __sub_stream_resolutions_avail[] = {
    [0] = { .Width = 704,  .Height = 576  },
    [1] = { .Width = 352,  .Height = 288  }
};

static ONVIF_PROFILE __onvif_profile[];

static ONVIF_VIDEO_ENCODER_CONFIG __onvif_main_video_encoder_config[] = {
	[0] = {
		.Name = "MainVideoEncoderConfig0",
		.token = "MainVideoEncoderConfigToken0",
		.UseCount = 1,
		.Encoding = tt__VideoEncoding__H264,
		.Resolution = { .Width = 1600, .Height = 1200 },
        .sizeResolutionsAvail = ARRAY_SIZE(__main_stream_resolutions_avail),
        .ResolutionsAvail = __main_stream_resolutions_avail,
		.Quality = 6.0,
		.RateControl = {
			.FrameRateLimit = 25,
			.EncodingInterval = 50,
			.BitrateLimit = 2048
		},
        .profile = &__onvif_profile[0]
	}
};

static ONVIF_VIDEO_ENCODER_CONFIG __onvif_sub_video_encoder_config[] = {
	[0] = {
		.Name = "SubVideoEncoderConfig0",
		.token = "SubVideoEncoderConfigToken0",
		.UseCount = 1,
		.Encoding = tt__VideoEncoding__H264,
		.Resolution = { .Width = 704, .Height = 576 },
        .sizeResolutionsAvail = ARRAY_SIZE(__sub_stream_resolutions_avail),
        .ResolutionsAvail = __sub_stream_resolutions_avail,
		.Quality = 3,
		.RateControl = {
			.FrameRateLimit = 10,
			.EncodingInterval = 10,
			.BitrateLimit = 256
		},
        .profile = &__onvif_profile[1]
	}
};

static ONVIF_PROFILE __onvif_profile[] = {
	[0] = {
		.Name = "MainProfile",
		.token = "main_profile",
		.VideoSourceConfig = &__onvif_video_source_config[0],
		.AudioSourceConfig = NULL,
		.VideoEncoderConfig = &__onvif_main_video_encoder_config[0],
		.AudioSourceConfig = NULL
	},
	[1] = {
		.Name = "SubProfile",
		.token = "sub_profile",
		.VideoSourceConfig = &__onvif_video_source_config[0],
		.AudioSourceConfig = NULL,
		.VideoEncoderConfig = &__onvif_sub_video_encoder_config[0],
		.AudioSourceConfig = NULL
	}
};

static ONVIF_PROFILES __onvif_profiles = {
	.numProfiles = ARRAY_SIZE(__onvif_profile),
	.Profiles = __onvif_profile
};

gboolean
onvif_resolution_name_to_value(const gchar *string, ONVIF_VIDEO_RESOLUTION *resolution)
{
    if (g_str_equal(string, "UXGA"))
    {
        resolution->Width = 1600;
        resolution->Height = 1200;
    }
    else if (g_str_equal(string, "1080P"))
    {
        resolution->Width = 1920;
        resolution->Height = 1080;
    }
    else if (g_str_equal(string, "960H"))
    {
        resolution->Width = 1280;
        resolution->Height = 960;
    }
    else if (g_str_equal(string, "720P"))
    {
        resolution->Width = 1280;
        resolution->Height = 720;
    }
    else if (g_str_equal(string, "D1"))
    {
        resolution->Width = 704;
        resolution->Height = 576;
    }
    else if (g_str_equal(string, "CIF"))
    {
        resolution->Width = 352;
        resolution->Height = 288;
    }
    else
    {
        g_warn_if_reached();
        return FALSE;
    }

    return TRUE;
}

const gchar *onvif_resolution_value_to_name(ONVIF_VIDEO_RESOLUTION *resolution)
{
    if (resolution->Width == 1600 && resolution->Height == 1200)
        return "UXGA";
    if (resolution->Width == 1920 && resolution->Height == 1080)
        return "1080P";
    if (resolution->Width == 1280 && resolution->Height == 960)
        return "960H";
    if (resolution->Width == 1280 && resolution->Height == 720)
        return "720P";
    if (resolution->Width == 704 && resolution->Height == 576)
        return "D1";
    if (resolution->Width == 352 && resolution->Height == 288)
        return "CIF";

    return "1080P";
}

gboolean
onvif_media_get_video_sources(struct soap *soap,
                              ONVIF_VIDEO_SOURCES **video_sources)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;

    *video_sources = &__onvif_video_sources;

    return TRUE;
}

gboolean
onvif_media_get_encoder_config(struct soap *soap,
                               char *configurationToken,
                               ONVIF_VIDEO_ENCODER_CONFIG **config)
{
	int i;

    if (!configurationToken)
        return FALSE;

    for (i = 0; i < ARRAY_SIZE(__onvif_main_video_encoder_config); i++) {
		if (strcmp(configurationToken, __onvif_main_video_encoder_config[i].token) == 0) {
			*config = &__onvif_main_video_encoder_config[i];
			return TRUE;
		}
	}
    for (i = 0; i < ARRAY_SIZE(__onvif_sub_video_encoder_config); i++) {
		if (strcmp(configurationToken, __onvif_sub_video_encoder_config[i].token) == 0) {
			*config = &__onvif_sub_video_encoder_config[i];
			return TRUE;
		}
	}

    return FALSE;
}

gboolean
onvif_media_get_profile(struct soap *soap,
                        char *token,
                        ONVIF_PROFILE **pprofile)
{
	JsonBuilder *builder;
	JsonNode *response;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
    ONVIF_PROFILE *profile;
	int i;

    ONVIF_DBG("%s: token=%s\n", __func__, token);

    profile = NULL;
    for (i = 0; i < ARRAY_SIZE(__onvif_profile); i++) {
		if (strcmp(token, __onvif_profile[i].token) == 0) {
			profile = &__onvif_profile[i];
		}
	}

    if (!profile)
        return FALSE;

	/* Request the Video setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "profile");
	json_builder_add_string_value(builder, "flip");
	json_builder_add_string_value(builder, "mirror");
	json_builder_add_string_value(builder, token);
	json_builder_end_array(builder);
	json_builder_end_object(builder);
    if (onvif_invocate_action(ionvif, "get_video", json_builder_get_root(builder), &response)) {
        JsonObject *item_obj = json_object_get_object_member(json_node_get_object(response), "items");
        if (item_obj) {
            if (json_object_has_member(item_obj, token)) {
                ONVIF_VIDEO_RESOLUTION resolution;
                const gchar *string;
                guint bitrate;
                guint framerate;
                JsonObject *pro_obj = json_object_get_object_member(item_obj, token);

                string = json_object_get_string_member(pro_obj, "resolution");
                bitrate = json_object_get_int_member(pro_obj, "bit_rate_value");
                framerate = json_object_get_int_member(pro_obj, "frame_rate");

                onvif_resolution_name_to_value(string, &resolution);

                /* Vide Source */
                __onvif_video_source[0].Framerate = framerate;
                __onvif_video_source[0].Resolution.Width = resolution.Width;
                __onvif_video_source[0].Resolution.Height = resolution.Height;
                /* Video Source Config */
                profile->VideoSourceConfig->Bounds.width = resolution.Width;
                profile->VideoSourceConfig->Bounds.height = resolution.Height;
                /* Video Encoder Config */
                profile->VideoEncoderConfig->Resolution.Width = resolution.Width;
                profile->VideoEncoderConfig->Resolution.Height = resolution.Height;
                profile->VideoEncoderConfig->RateControl.FrameRateLimit = framerate;
                profile->VideoEncoderConfig->RateControl.BitrateLimit = bitrate;
            }
        }
        json_node_free(response);
    }

    *pprofile = profile;

    return TRUE;
}

gboolean
onvif_media_get_profiles(struct soap *soap,
                         ONVIF_PROFILES **profiles)
{
    ONVIF_PROFILE *profile;
    int i;

    for (i = 0; i < __onvif_profiles.numProfiles; i++) {
        onvif_media_get_profile(soap, __onvif_profiles.Profiles[i].token, &profile);
    }

    *profiles = &__onvif_profiles;
	return TRUE;
}

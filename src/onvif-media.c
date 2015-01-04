#include "soapH.h"
#include "onvif_impl.h"
#include "onvif-auth.h"

int
__trt__GetServiceCapabilities(struct soap *soap,
                              struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities,
                              struct _trt__GetServiceCapabilitiesResponse *trt__GetServiceCapabilitiesResponse)
{
    struct _trt__GetServiceCapabilitiesResponse *Response = trt__GetServiceCapabilitiesResponse;

    ONVIF_DBG("\n");

    SOAP_CALLOC_1(soap, Response->Capabilities);
    SOAP_CALLOC_1(soap, Response->Capabilities->ProfileCapabilities);
    SOAP_CALLOC_1(soap, Response->Capabilities->ProfileCapabilities->MaximumNumberOfProfiles);
    *Response->Capabilities->ProfileCapabilities->MaximumNumberOfProfiles = 2;

    SOAP_CALLOC_1(soap, Response->Capabilities->StreamingCapabilities);
    SOAP_CALLOC_1(soap, Response->Capabilities->StreamingCapabilities->RTPMulticast);
    *Response->Capabilities->StreamingCapabilities->RTPMulticast = xsd__boolean__false_;
    SOAP_CALLOC_1(soap, Response->Capabilities->StreamingCapabilities->RTP_USCORETCP);
    *Response->Capabilities->StreamingCapabilities->RTP_USCORETCP = xsd__boolean__true_;
    SOAP_CALLOC_1(soap, Response->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP);
    *Response->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = xsd__boolean__true_;
    SOAP_CALLOC_1(soap, Response->Capabilities->StreamingCapabilities->NonAggregateControl);
    *Response->Capabilities->StreamingCapabilities->NonAggregateControl = xsd__boolean__false_;
    SOAP_CALLOC_1(soap, Response->Capabilities->StreamingCapabilities->NoRTSPStreaming);
    *Response->Capabilities->StreamingCapabilities->NoRTSPStreaming = xsd__boolean__false_;

    SOAP_CALLOC_1(soap, Response->Capabilities->SnapshotUri);
    *Response->Capabilities->SnapshotUri = xsd__boolean__true_;

    SOAP_CALLOC_1(soap, Response->Capabilities->Rotation);
    *Response->Capabilities->Rotation = xsd__boolean__true_;

    SOAP_CALLOC_1(soap, Response->Capabilities->VideoSourceMode);
    *Response->Capabilities->VideoSourceMode = xsd__boolean__false_;

    SOAP_CALLOC_1(soap, Response->Capabilities->OSD);
    *Response->Capabilities->OSD = xsd__boolean__true_;

    return SOAP_OK;
}

int
__trt__GetVideoSources(struct soap* soap,
                       struct _trt__GetVideoSources *trt__GetVideoSources,
                       struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
	ONVIF_VIDEO_SOURCES *video_sources = NULL;
	int i;

    ONVIF_DBG("\n");
	ACCESS_CONTROL;

	if (!onvif_media_get_video_sources(soap, &video_sources))
		return SOAP_NO_DATA;

	trt__GetVideoSourcesResponse->__sizeVideoSources = video_sources->numVideoSources;

	SOAP_CALLOC(soap, trt__GetVideoSourcesResponse->VideoSources, video_sources->numVideoSources); 

	for (i = 0; i < video_sources->numVideoSources; i++) {
		ONVIF_VIDEO_SOURCE *vs = &video_sources->VideoSources[i];

		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Resolution);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Brightness);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->ColorSaturation);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Contrast);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->IrCutFilter);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Sharpness);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->BacklightCompensation);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange);
		SOAP_CALLOC_1(soap, trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance);

		trt__GetVideoSourcesResponse->VideoSources[i].Framerate = vs->Framerate;
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Height = vs->Resolution.Height;
		trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Width = vs->Resolution.Width;
		SOAP_SET_STRING_FIELD(soap, trt__GetVideoSourcesResponse->VideoSources[i].token, vs->token);

		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Brightness[0] = vs->Imaging.Brightness;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->ColorSaturation[0] = vs->Imaging.ColorSaturation;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Contrast[0] = vs->Imaging.Contrast;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->IrCutFilter[0] = vs->Imaging.IrCutFilter;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->Sharpness[0] = vs->Imaging.Sharpness;

		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->BacklightCompensation->Mode = vs->Imaging.BacklightCompensation.Mode;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->BacklightCompensation->Level = vs->Imaging.BacklightCompensation.Level;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange->Mode = vs->Imaging.WideDynamicRange.Mode;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WideDynamicRange->Level = vs->Imaging.WideDynamicRange.Level;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->Mode = vs->Imaging.WhiteBalance.Mode;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->CrGain = vs->Imaging.WhiteBalance.CrGain;
		trt__GetVideoSourcesResponse->VideoSources[i].Imaging->WhiteBalance->CbGain = vs->Imaging.WhiteBalance.CbGain;
	}

	return SOAP_OK;
}

int __trt__GetProfile(struct soap* soap,
		struct _trt__GetProfile *trt__GetProfile,
		struct _trt__GetProfileResponse *trt__GetProfileResponse)
{
	ONVIF_PROFILE *profile = NULL;

    ONVIF_DBG("%s(ProfileToken=%s)\n", __func__, trt__GetProfile->ProfileToken);
	ACCESS_CONTROL;

	if (!onvif_media_get_profile(soap, trt__GetProfile->ProfileToken, &profile))
		return SOAP_NO_DATA;

	SOAP_CALLOC_1(soap, trt__GetProfileResponse->Profile);
	SOAP_CALLOC_1(soap, trt__GetProfileResponse->Profile->VideoSourceConfiguration);
	SOAP_CALLOC_1(soap, trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds);

	SOAP_SET_STRING_FIELD(soap, trt__GetProfileResponse->Profile->Name,
	                      profile->Name);
	SOAP_SET_STRING_FIELD(soap, trt__GetProfileResponse->Profile->token,
	                      profile->token);
	SOAP_SET_STRING_FIELD(soap, trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name,
	                      profile->VideoSourceConfig->Name);
	SOAP_SET_STRING_FIELD(soap, trt__GetProfileResponse->Profile->VideoSourceConfiguration->token,
	                      profile->VideoSourceConfig->token);
	SOAP_SET_STRING_FIELD(soap, trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken,
	                      profile->VideoSourceConfig->SourceToken);

	trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount = profile->VideoSourceConfig->UseCount;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->x = profile->VideoSourceConfig->Bounds.x;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->y = profile->VideoSourceConfig->Bounds.y;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->height = profile->VideoSourceConfig->Bounds.height;
	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->width = profile->VideoSourceConfig->Bounds.width;

	SOAP_CALLOC_1(soap, trt__GetProfileResponse->Profile->VideoEncoderConfiguration);
	SOAP_CALLOC_1(soap, trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution);
	SOAP_CALLOC_1(soap, trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl);

	SOAP_SET_STRING_FIELD(soap, trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name,
	                      profile->VideoEncoderConfig->Name);
	SOAP_SET_STRING_FIELD(soap, trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token,
	                      profile->VideoEncoderConfig->token);
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount = profile->VideoEncoderConfig->UseCount;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Quality = profile->VideoEncoderConfig->Quality;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding = profile->VideoEncoderConfig->Encoding;//JPEG = 0, MPEG4 = 1, H264 = 2;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Height = profile->VideoEncoderConfig->Resolution.Height;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Width = profile->VideoEncoderConfig->Resolution.Width;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit = profile->VideoEncoderConfig->RateControl.FrameRateLimit;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval = profile->VideoEncoderConfig->RateControl.EncodingInterval;
	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit = profile->VideoEncoderConfig->RateControl.BitrateLimit;

	return SOAP_OK;
}

int __trt__GetProfiles(struct soap* soap,
		struct _trt__GetProfiles *trt__GetProfiles,
		struct _trt__GetProfilesResponse *trt__GetProfilesResponse)
{
	ONVIF_PROFILES *profiles = NULL;
	int i;

    ONVIF_DBG("\n");
	ACCESS_CONTROL;

	if (!onvif_media_get_profiles(soap, &profiles))
		return SOAP_NO_DATA;

	trt__GetProfilesResponse->__sizeProfiles = profiles->numProfiles;
	SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles, profiles->numProfiles);

	for (i = 0; i < profiles->numProfiles; i++) {
		ONVIF_PROFILE *p = &profiles->Profiles[i];

		SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration, 1);
		SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds, 1);

		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].Name, p->Name);
		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].token, p->token);
		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name, p->VideoSourceConfig->Name);
		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token, p->VideoSourceConfig->token);
		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken, p->VideoSourceConfig->SourceToken);

		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->UseCount, p->VideoSourceConfig->UseCount);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->x, p->VideoSourceConfig->Bounds.x);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->y, p->VideoSourceConfig->Bounds.y);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->height, p->VideoSourceConfig->Bounds.height);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->width, p->VideoSourceConfig->Bounds.width);

		SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration, 1);
		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name, p->VideoEncoderConfig->Name);
		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token, p->VideoEncoderConfig->token);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->UseCount, p->VideoEncoderConfig->UseCount);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Quality, p->VideoEncoderConfig->Quality);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Encoding, p->VideoEncoderConfig->Encoding);

        SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution, 1);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Height, p->VideoEncoderConfig->Resolution.Height);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Width, p->VideoEncoderConfig->Resolution.Width);

        SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl, 1);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit, p->VideoEncoderConfig->RateControl.FrameRateLimit);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->EncodingInterval, p->VideoEncoderConfig->RateControl.EncodingInterval);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->BitrateLimit, p->VideoEncoderConfig->RateControl.BitrateLimit);
	}

	return SOAP_OK;
}


/** Auto-test server operation __trt__GetStreamUri */
int __trt__GetStreamUri(struct soap *soap,
                        struct _trt__GetStreamUri *trt__GetStreamUri,
                        struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse)
{
	JsonBuilder *builder;
	JsonNode *response;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	char *uri;
	char *key;
	const char *ipaddr;
    guint port;
	char *path = NULL;

    ONVIF_DBG("\n");
	ACCESS_CONTROL;

	/* Request the Network setting */
	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_array(builder);
	json_builder_add_string_value(builder, "address");
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	if (onvif_invocate_action(ionvif, "get_network", json_builder_get_root(builder), &response)) {
		JsonObject *items_obj;
		JsonObject *addr_obj;

		items_obj = json_object_get_object_member(json_node_get_object(response), "items");
		addr_obj = json_object_get_object_member(items_obj, "address");
		if (addr_obj) {
			ipaddr = json_object_get_string_member(addr_obj, "ipaddr");
		}
		json_node_free(response);
	}
    else {
        ipaddr = ipcam_ionvif_get_int_property(ionvif, "network:address:ipaddr");
    }
    g_object_unref(builder);

	if (asprintf(&key, "video:%s:stream_path", trt__GetStreamUri->ProfileToken) > 0) {
		path = (char *)ipcam_ionvif_get_string_property(ionvif, key);
		free(key);
	}

	port = ipcam_ionvif_get_int_property(ionvif, "network:port:rtsp");
	asprintf(&uri, "rtsp://%s:%d/%s",
	         ipaddr,
	         port, 
	         path);

	SOAP_CALLOC(soap, trt__GetStreamUriResponse->MediaUri, 1);
	SOAP_SET_STRING_FIELD(soap, trt__GetStreamUriResponse->MediaUri->Uri, uri);

	free(uri);

	return SOAP_OK;
}


int
__trt__AddVideoEncoderConfiguration(struct soap *soap,
                                    struct _trt__AddVideoEncoderConfiguration *trt__AddVideoEncoderConfiguration,
                                    struct _trt__AddVideoEncoderConfigurationResponse *trt__AddVideoEncoderConfigurationResponse)
{
    ONVIF_DBG("\n");
	return SOAP_OK;
}


int
__trt__RemoveVideoEncoderConfiguration(struct soap *soap,
                                       struct _trt__RemoveVideoEncoderConfiguration *trt__RemoveVideoEncoderConfiguration,
                                       struct _trt__RemoveVideoEncoderConfigurationResponse *trt__RemoveVideoEncoderConfigurationResponse)
{
    ONVIF_DBG("\n");
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


int
__trt__GetVideoEncoderConfigurations(struct soap *soap,
                                     struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations,
                                     struct _trt__GetVideoEncoderConfigurationsResponse *trt__GetVideoEncoderConfigurationsResponse)
{
    ONVIF_DBG("\n");
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


int
__trt__GetVideoEncoderConfiguration(struct soap *soap,
                                    struct _trt__GetVideoEncoderConfiguration *trt__GetVideoEncoderConfiguration,
                                    struct _trt__GetVideoEncoderConfigurationResponse *trt__GetVideoEncoderConfigurationResponse)
{
    ONVIF_DBG("\n");
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


int
__trt__GetCompatibleVideoEncoderConfigurations(struct soap *soap,
                                               struct _trt__GetCompatibleVideoEncoderConfigurations *trt__GetCompatibleVideoEncoderConfigurations,
                                               struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *trt__GetCompatibleVideoEncoderConfigurationsResponse)
{
    struct _trt__GetCompatibleVideoEncoderConfigurations *Request = trt__GetCompatibleVideoEncoderConfigurations;
    struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *Response = trt__GetCompatibleVideoEncoderConfigurationsResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
    ONVIF_PROFILE *profile = NULL;
    ONVIF_VIDEO_ENCODER_CONFIG *encoder_config = NULL;
    int i;

    ONVIF_DBG("\n");
	ACCESS_CONTROL;

	if (!onvif_media_get_profile(soap, Request->ProfileToken, &profile))
		return SOAP_NO_DATA;

    encoder_config = profile->VideoEncoderConfig;

    Response->__sizeConfigurations = 1;
    SOAP_CALLOC_1(soap, Response->Configurations);
    SOAP_SET_STRING_FIELD(soap, Response->Configurations->Name, encoder_config->Name);
    SOAP_SET_STRING_FIELD(soap, Response->Configurations->token, encoder_config->token);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->UseCount, 1);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->Encoding, tt__VideoEncoding__H264);
    SOAP_CALLOC_1(soap, Response->Configurations->Resolution);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->Resolution->Width, encoder_config->Resolution.Width);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->Resolution->Height, encoder_config->Resolution.Height);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->Quality, 6.0);
    SOAP_CALLOC_1(soap, Response->Configurations->RateControl);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->RateControl->FrameRateLimit, encoder_config->RateControl.FrameRateLimit);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->RateControl->EncodingInterval, encoder_config->RateControl.EncodingInterval);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->RateControl->BitrateLimit, encoder_config->RateControl.BitrateLimit);
    SOAP_CALLOC_1(soap, Response->Configurations->H264);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->H264->GovLength, 30);
    SOAP_SET_VALUE_FIELD(soap, Response->Configurations->H264->H264Profile, tt__H264Profile__Baseline);

    return SOAP_OK;
}


int
__trt__SetVideoEncoderConfiguration(struct soap *soap,
                                    struct _trt__SetVideoEncoderConfiguration *trt__SetVideoEncoderConfiguration,
                                    struct _trt__SetVideoEncoderConfigurationResponse *trt__SetVideoEncoderConfigurationResponse)
{
    JsonBuilder *builder;
    JsonNode *response;
    struct _trt__SetVideoEncoderConfiguration *Request = trt__SetVideoEncoderConfiguration;
    struct _trt__SetVideoEncoderConfigurationResponse *Response = trt__SetVideoEncoderConfigurationResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
    ONVIF_VIDEO_ENCODER_CONFIG *encoder_config;

    if (!onvif_media_get_encoder_config(soap, Request->Configuration->token, &encoder_config))
        return SOAP_NO_DATA;

    builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, encoder_config->profile->token);
    json_builder_begin_object(builder);

    if (Request->Configuration->Resolution) {
        ONVIF_VIDEO_RESOLUTION resolution = {
            .Width = Request->Configuration->Resolution->Width,
            .Height = Request->Configuration->Resolution->Height
        };
        const char *string = onvif_resolution_value_to_name(&resolution);

        json_builder_set_member_name(builder, "resolution");
        json_builder_add_string_value(builder, string);
    }
    if (Request->Configuration->RateControl) {
        json_builder_set_member_name(builder, "frame_rate");
        json_builder_add_int_value(builder, Request->Configuration->RateControl->FrameRateLimit);
        json_builder_set_member_name(builder, "bit_rate_value");
        json_builder_add_int_value(builder, Request->Configuration->RateControl->BitrateLimit);
    }
    json_builder_end_object(builder);
    json_builder_end_object(builder);
    json_builder_end_object(builder);

    if (onvif_invocate_action(ionvif, "set_video", json_builder_get_root(builder), &response)) {
        json_node_free(response);
    }

    g_object_unref(builder);

    return SOAP_OK;
}


int
__trt__GetVideoSourceConfigurations(struct soap *soap,
                                    struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations,
                                    struct _trt__GetVideoSourceConfigurationsResponse *trt__GetVideoSourceConfigurationsResponse)
{
    struct _trt__GetVideoSourceConfigurationsResponse *Response = trt__GetVideoSourceConfigurationsResponse;

    ONVIF_DBG("\n");

    return SOAP_OK;
}


int
__trt__GetVideoEncoderConfigurationOptions(struct soap *soap,
                                           struct _trt__GetVideoEncoderConfigurationOptions *trt__GetVideoEncoderConfigurationOptions,
                                           struct _trt__GetVideoEncoderConfigurationOptionsResponse *trt__GetVideoEncoderConfigurationOptionsResponse)
{
    struct _trt__GetVideoEncoderConfigurationOptions *Request = trt__GetVideoEncoderConfigurationOptions;
    struct _trt__GetVideoEncoderConfigurationOptionsResponse *Response = trt__GetVideoEncoderConfigurationOptionsResponse;
    ONVIF_VIDEO_ENCODER_CONFIG *encoder_config = NULL;
    int i;

    ONVIF_DBG("\n");
	ACCESS_CONTROL;

	if (!onvif_media_get_encoder_config(soap, trt__GetVideoEncoderConfigurationOptions->ConfigurationToken, &encoder_config))
		return SOAP_NO_DATA;

    SOAP_CALLOC_1(soap, Response->Options);
    SOAP_CALLOC_1(soap, Response->Options->QualityRange);
    Response->Options->QualityRange->Min = 0;
    Response->Options->QualityRange->Max = 255;

    SOAP_CALLOC_1(soap, Response->Options->H264);
    Response->Options->H264->__sizeResolutionsAvailable = encoder_config->sizeResolutionsAvail;
    SOAP_CALLOC(soap, Response->Options->H264->ResolutionsAvailable, encoder_config->sizeResolutionsAvail);
    for (i = 0; i < encoder_config->sizeResolutionsAvail; i++) {
        Response->Options->H264->ResolutionsAvailable[i].Width = encoder_config->ResolutionsAvail[i].Width;
        Response->Options->H264->ResolutionsAvailable[i].Height = encoder_config->ResolutionsAvail[i].Height;
    }
    SOAP_CALLOC_1(soap, Response->Options->H264->GovLengthRange);
    Response->Options->H264->GovLengthRange->Min = 1;
    Response->Options->H264->GovLengthRange->Max = 65536;
    SOAP_CALLOC_1(soap, Response->Options->H264->FrameRateRange);
    Response->Options->H264->FrameRateRange->Min = 1;
    Response->Options->H264->FrameRateRange->Max = 30;
    Response->Options->H264->__sizeH264ProfilesSupported = 3;
    SOAP_CALLOC(soap, Response->Options->H264->H264ProfilesSupported, 3);
    Response->Options->H264->H264ProfilesSupported[0] = tt__H264Profile__Baseline;
    Response->Options->H264->H264ProfilesSupported[1] = tt__H264Profile__Main;
    Response->Options->H264->H264ProfilesSupported[2] = tt__H264Profile__High;

	return SOAP_OK;
}


int __timg__GetOptions(struct soap *soap,
                       struct _timg__GetOptions *timg__GetOptions,
                       struct _timg__GetOptionsResponse *timg__GetOptionsResponse)
{
	struct _timg__GetOptionsResponse *Response = timg__GetOptionsResponse;

    ONVIF_DBG("\n");
	ACCESS_CONTROL;

	SOAP_CALLOC_1(soap, Response->ImagingOptions);
	SOAP_CALLOC_1(soap, Response->ImagingOptions->Brightness);
	SOAP_CALLOC_1(soap, Response->ImagingOptions->ColorSaturation);
	SOAP_CALLOC_1(soap, Response->ImagingOptions->Contrast);

	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Brightness->Min, 0.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Brightness->Max, 100.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->ColorSaturation->Min, 0.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->ColorSaturation->Max, 100.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Contrast->Min, 0.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Contrast->Max, 100.0);

	return SOAP_OK;
}


int __timg__GetImagingSettings(struct soap *soap,
                               struct _timg__GetImagingSettings *timg__GetImagingSettings,
                               struct _timg__GetImagingSettingsResponse *timg__GetImagingSettingsResponse)
{
    JsonBuilder *builder;
    JsonNode *response;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	struct _timg__GetImagingSettingsResponse *Response = timg__GetImagingSettingsResponse;

    ONVIF_DBG("\n");
	ACCESS_CONTROL;

    builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_array(builder);
    json_builder_add_string_value(builder, "brightness");
    json_builder_add_string_value(builder, "chrominance");
    json_builder_add_string_value(builder, "contrast");
    json_builder_add_string_value(builder, "saturation");
    json_builder_end_array(builder);
    json_builder_end_object(builder);

    if (onvif_invocate_action(ionvif, "get_image", json_builder_get_root(builder), &response)) {
        JsonObject *item_obj = json_object_get_object_member(json_node_get_object(response), "items");

        SOAP_CALLOC_1(soap, Response->ImagingSettings);
        if (json_object_has_member(item_obj, "brightness")) {
            float value = (float)json_object_get_int_member(item_obj, "brightness");

            SOAP_CALLOC_1(soap, Response->ImagingSettings->Brightness);
            SOAP_SET_VALUE_FIELD(soap, *Response->ImagingSettings->Brightness, value);
        }
        if (json_object_has_member(item_obj, "saturation")) {
            float value = (float)json_object_get_int_member(item_obj, "saturation");

            SOAP_CALLOC_1(soap, Response->ImagingSettings->ColorSaturation);
            SOAP_SET_VALUE_FIELD(soap, *Response->ImagingSettings->ColorSaturation, value);
        }
        if (json_object_has_member(item_obj, "contrast")) {
            float value = (float)json_object_get_int_member(item_obj, "contrast");

            SOAP_CALLOC_1(soap, Response->ImagingSettings->Contrast);
            SOAP_SET_VALUE_FIELD(soap, *Response->ImagingSettings->Contrast, value);
        }
    }
    g_object_unref(builder);

	return SOAP_OK;
}


int __timg__SetImagingSettings(struct soap *soap,
                               struct _timg__SetImagingSettings *timg__SetImagingSettings,
                               struct _timg__SetImagingSettingsResponse *timg__SetImagingSettingsResponse)
{
    JsonBuilder *builder;
    JsonNode *response;
    struct _timg__SetImagingSettings *Request = timg__SetImagingSettings;
    struct _timg__SetImagingSettingsResponse *Response = timg__SetImagingSettingsResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;

    ONVIF_DBG("VideoSourceToken=%s\n", Request->VideoSourceToken);
	ACCESS_CONTROL;

    builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "items");
    json_builder_begin_object(builder);
    if (Request->ImagingSettings->Brightness) {
        json_builder_set_member_name(builder, "brightness");
        json_builder_add_int_value(builder, (guint64)*Request->ImagingSettings->Brightness);
    }
    if (Request->ImagingSettings->ColorSaturation) {
        json_builder_set_member_name(builder, "saturation");
        json_builder_add_int_value(builder, (guint64)*Request->ImagingSettings->ColorSaturation);
    }
    if (Request->ImagingSettings->Contrast) {
        json_builder_set_member_name(builder, "contrast");
        json_builder_add_int_value(builder, (guint64)*Request->ImagingSettings->Contrast);
    }
    json_builder_end_object(builder);
    json_builder_end_object(builder);

    if (onvif_invocate_action(ionvif, "set_image", json_builder_get_root(builder), &response)) {
        json_node_free(response);
    }

    g_object_unref(builder);

    return SOAP_OK;
}


/** Auto-test server operation __timg__Move */
int __timg__Move(struct soap *soap,
                 struct _timg__Move *timg__Move,
                 struct _timg__MoveResponse *timg__MoveResponse)
{
    ONVIF_DBG("\n");
	ACCESS_CONTROL;

    return SOAP_OK;
}


/** Auto-test server operation __timg__Stop */
int __timg__Stop(struct soap *soap,
                 struct _timg__Stop *timg__Stop,
                 struct _timg__StopResponse *timg__StopResponse)
{
    ONVIF_DBG("\n");
	ACCESS_CONTROL;

    return SOAP_OK;
}


/** Auto-test server operation __timg__GetStatus */
int __timg__GetStatus(struct soap *soap,
                      struct _timg__GetStatus *timg__GetStatus,
                      struct _timg__GetStatusResponse *timg__GetStatusResponse)
{
    ONVIF_DBG("\n");
	ACCESS_CONTROL;

    /* Return incomplete response with default data values */
	return SOAP_OK;
}


/** Auto-test server operation __timg__GetMoveOptions */
int __timg__GetMoveOptions(struct soap *soap,
                           struct _timg__GetMoveOptions *timg__GetMoveOptions,
                           struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse)
{
    ONVIF_DBG("\n");
	ACCESS_CONTROL;

    /* Return incomplete response with default data values */
	return SOAP_OK;
}

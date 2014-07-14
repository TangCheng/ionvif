#include "soapH.h"
#include "onvif_impl.h"
#include "onvif-auth.h"

int __trt__GetVideoSources(struct soap* soap,
		struct _trt__GetVideoSources *trt__GetVideoSources,
		struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
	ONVIF_VIDEO_SOURCES *video_sources = NULL;
	int i;

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

	ACCESS_CONTROL;

	printf("token:%s\n", trt__GetProfile->ProfileToken);

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
		SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution, 1);
		SOAP_CALLOC(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl, 1);

		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name, p->VideoEncoderConfig->Name);
		SOAP_SET_STRING_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token, p->VideoEncoderConfig->token);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->UseCount, p->VideoEncoderConfig->UseCount);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Quality, p->VideoEncoderConfig->Quality);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Encoding, p->VideoEncoderConfig->Encoding);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Height, p->VideoEncoderConfig->Resolution.Height);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Width, p->VideoEncoderConfig->Resolution.Width);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit, p->VideoEncoderConfig->RateControl.FrameRateLimit);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->EncodingInterval, p->VideoEncoderConfig->RateControl.EncodingInterval);
		SOAP_SET_VALUE_FIELD(soap, trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->BitrateLimit, p->VideoEncoderConfig->RateControl.BitrateLimit);
	}

	printf("GetProfiles\n");

	return SOAP_OK;
}


/** Auto-test server operation __trt__GetStreamUri */
int __trt__GetStreamUri(struct soap *soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	char *uri;
	char *key;
	char *ipaddr;
	char *path = NULL;

	ACCESS_CONTROL;

	if (asprintf(&key, "onvif:profile:%s", trt__GetStreamUri->ProfileToken) > 0) {
		path = (char *)ipcam_base_app_get_config(IPCAM_BASE_APP(ionvif), key);
		free(key);
	}
	ipaddr = ipcam_ionvif_get_server_addr(ionvif);
	asprintf(&uri, "rtsp://%s:%d/%s",
	         ipaddr,
	         ipcam_ionvif_get_rtsp_port (ionvif), 
	         path);
	free(ipaddr);

	SOAP_CALLOC(soap, trt__GetStreamUriResponse->MediaUri, 1);
	SOAP_SET_STRING_FIELD(soap, trt__GetStreamUriResponse->MediaUri->Uri, uri);

	free(uri);

	return SOAP_OK;
}


int __timg__GetOptions(struct soap *soap, struct _timg__GetOptions *timg__GetOptions, struct _timg__GetOptionsResponse *timg__GetOptionsResponse)
{
	struct _timg__GetOptionsResponse *Response = timg__GetOptionsResponse;

	SOAP_CALLOC_1(soap, Response->ImagingOptions);
	SOAP_CALLOC_1(soap, Response->ImagingOptions->Brightness);
	SOAP_CALLOC_1(soap, Response->ImagingOptions->ColorSaturation);
	SOAP_CALLOC_1(soap, Response->ImagingOptions->Contrast);

	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Brightness->Min, 0.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Brightness->Max, 255.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->ColorSaturation->Min, 0.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->ColorSaturation->Max, 255.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Contrast->Min, 0.0);
	SOAP_SET_VALUE_FIELD(soap, Response->ImagingOptions->Contrast->Max, 7.0);

	return SOAP_OK;
}


int __timg__GetImagingSettings(struct soap *soap, struct _timg__GetImagingSettings *timg__GetImagingSettings, struct _timg__GetImagingSettingsResponse *timg__GetImagingSettingsResponse)
{
	struct _timg__GetImagingSettingsResponse *Response = timg__GetImagingSettingsResponse;

	printf("%s(%s)\n", __func__, timg__GetImagingSettings->VideoSourceToken);

	SOAP_CALLOC_1(soap, Response->ImagingSettings);
	SOAP_CALLOC_1(soap, Response->ImagingSettings->Brightness);
	SOAP_CALLOC_1(soap, Response->ImagingSettings->ColorSaturation);
	SOAP_CALLOC_1(soap, Response->ImagingSettings->Contrast);
	SOAP_SET_VALUE_FIELD(soap, *Response->ImagingSettings->Brightness, 128.0);
	SOAP_SET_VALUE_FIELD(soap, *Response->ImagingSettings->ColorSaturation, 128.0);
	SOAP_SET_VALUE_FIELD(soap, *Response->ImagingSettings->Contrast, 4.0);

	return SOAP_OK;
}


int __timg__SetImagingSettings(struct soap *soap, struct _timg__SetImagingSettings *timg__SetImagingSettings, struct _timg__SetImagingSettingsResponse *timg__SetImagingSettingsResponse)
{	
	return SOAP_OK;
}


/** Auto-test server operation __timg__Move */
int __timg__Move(struct soap *soap, struct _timg__Move *timg__Move, struct _timg__MoveResponse *timg__MoveResponse)
{	
	return SOAP_OK;
}


/** Auto-test server operation __timg__Stop */
int __timg__Stop(struct soap *soap, struct _timg__Stop *timg__Stop, struct _timg__StopResponse *timg__StopResponse)
{	
	return SOAP_OK;
}


/** Auto-test server operation __timg__GetStatus */
int __timg__GetStatus(struct soap *soap, struct _timg__GetStatus *timg__GetStatus, struct _timg__GetStatusResponse *timg__GetStatusResponse)
{	
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


/** Auto-test server operation __timg__GetMoveOptions */
int __timg__GetMoveOptions(struct soap *soap, struct _timg__GetMoveOptions *timg__GetMoveOptions, struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse)
{	
	/* Return incomplete response with default data values */
	return SOAP_OK;
}

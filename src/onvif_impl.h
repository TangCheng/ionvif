#ifndef __ONVIF_IMPL_H__
#define __ONVIF_IMPL_H__

#include <time.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>
#include "ionvif.h"

#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x[0]))

#define DBG_LINE printf("enter: %s\n", __FUNCTION__);

#define SOAP_CALLOC(soap, ptr, cnt) \
do { \
	size_t __size = sizeof(*ptr) * cnt; \
	ptr = soap_malloc(soap, __size); \
	memset(ptr, 0, __size); \
} while(0)

#define SOAP_CALLOC_1(soap, ptr)	SOAP_CALLOC(soap, ptr, 1)

#define SOAP_SET_STRING_FIELD(soap, var, val) \
do { \
	var = soap_strdup(soap, val); \
} while(0)

#define SOAP_SET_VALUE_FIELD(soap, var, val) \
do { \
	var = val; \
} while(0)

#define SOAP_GET_VALUE_FIELD(soap, var, val) \
do { \
	val = var; \
} while(0)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ONVIF_VIDEO_CONFIG
{
	char *Name;
	int UseCount;
	char *token;
	char *SourceToken;
	struct {
		int x;
		int y;
		int width;
		int height;
	} Bounds;
} ONVIF_VIDEO_CONFIG;

typedef struct ONVIF_AUDIO_CONFIG{
	char *Name;
	int UseCount;
	char *token;
	char *SourceToken;
} ONVIF_AUDIO_CONFIG;

typedef struct ONVIF_VIDEO_ENCODER_CONFIG {
	char *Name;
	int UseCount;
	char *token;
	int Encoding;   /* 0: JPEG, 1: MPEG4, 2: H264 */
	struct {
		int Width;
		int Height;
	} Resolution;
	float Quality;
	struct {
		int FrameRateLimit;
		int EncodingInterval;
		int BitrateLimit;
	} RateControl;
} ONVIF_VIDEO_ENCODER_CONFIG;

typedef struct ONVIF_AUDIO_ENCODER_CONFIG {
	char *Name;
	int UseCount;
	char *token;
	int Encoding;   /* 0: G711, 1: G726, 2: AAC */
	int Bitrate;
	int SampleRate;
} ONVIF_AUDIO_ENCODER_CONFIG;

typedef struct ONVIF_PROFILE
{
	char *Name;
	char *token;
	ONVIF_VIDEO_CONFIG *VideoSourceConfig;
	ONVIF_AUDIO_CONFIG *AudioSourceConfig;
	ONVIF_VIDEO_ENCODER_CONFIG *VideoEncoderConfig;
	ONVIF_AUDIO_ENCODER_CONFIG *AudioEncoderConfig;
} ONVIF_PROFILE;

typedef struct ONVIF_PROFILES
{
	int numProfiles;
	ONVIF_PROFILE *Profiles;
} ONVIF_PROFILES;

/* ONVIF_VIDEO_SOURCE / ONVIF_VIDEO_SOURCES */

typedef struct ONVIF_VIDEO_SOURCE
{
	char *token;
	int Framerate;
	/* Resolution */
	struct {
		int Width;
		int Height;
	} Resolution;
	/* Imaging */
	struct {
		struct {	/* BacklightCompensation */
			int Mode;
			float Level;
		} BacklightCompensation;
		float Brightness;
		float ColorSaturation;
		float Contrast;
		int IrCutFilter;
		float Sharpness;
		struct {
			int Mode;
			float Level;
		} WideDynamicRange;
		struct {
			int Mode;
			float CrGain;
			float CbGain;
		} WhiteBalance;
	} Imaging;
} ONVIF_VIDEO_SOURCE;

typedef struct ONVIF_VIDEO_SOURCES {
	int numVideoSources;
	ONVIF_VIDEO_SOURCE *VideoSources;
} ONVIF_VIDEO_SOURCES;

/* Profiles */
gboolean onvif_media_get_profile(struct soap *soap,
                                 char *token,
                                 ONVIF_PROFILE **profile);
gboolean onvif_media_get_profiles(struct soap *soap,
                                  ONVIF_PROFILES **profiles);
/* Video Sources */
gboolean onvif_media_get_video_sources(struct soap *soap,
                                      ONVIF_VIDEO_SOURCES **video_sources);

char *onvif_media_get_stream_uri(struct soap *soap, char *profile_token);

gboolean onvif_invocate_action(IpcamIOnvif *ionvif, const char *action,
                               JsonNode *request, JsonNode **response);

#endif

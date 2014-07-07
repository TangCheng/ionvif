#ifndef __ONVIF_IMPL_H__
#define __ONVIF_IMPL_H__

#include <time.h>
#include <glib-object.h>
#include "ionvif.h"

#define DBG_LINE printf("enter: %s\n", __FUNCTION__);
#define DBG_TAG(l) printf("dbg:%s %s\n", __FUNCTION__, l);


#define CHECK_FIELD(em) \
do { \
	if (em == NULL) { \
		size_t size = sizeof(*em); \
		em = soap_malloc(soap, size); \
		memset(em, 0, size); \
	}\
} while(0)

#define SOAP_CALLOC(soap, ptr, cnt) \
do { \
	size_t __size = sizeof(*ptr) * cnt; \
	ptr = soap_malloc(soap, __size); \
	memset(ptr, 0, __size); \
} while(0)

#define SOAP_SET_STRING_FIELD(soap, var, val) \
do { \
	var = soap_strdup(soap, val); \
} while(0)

#define SOAP_SET_VALUE_FIELD(soap, var, val) \
do { \
	var = val; \
} while(0)

#define LOAD_FIELD(obj1, name1, obj2, name2) \
	(obj1).(name1) = (obj2).(name2);

#define STORE_FIELD(obj1, name1, obj2, name2) \
	 (obj2).(name2) = (obj1).(name1);



#define LOAD(val1, val2) \
	val1 = val2;

#define STORE(val1, val2) \
	 val2 = val1;

struct _OnvifServerContext;
typedef struct _OnvifServerContext OnvifServerContext; 

OnvifServerContext *onvif_server_context_new(IpcamIOnvif *ionvif);
void onvif_server_context_destroy(OnvifServerContext *ctx);

void soap_set_field_string(struct soap* soap, char ** p_field, const char* val);

int onvif_dm_get_scopes(struct soap *soap, char ***scopes);
void onvif_dm_set_scopes(struct soap *soap, int nr_scopes, char **scopes);

const char * onvif_dm_get_ipv4_address(struct soap* soap);
const char * onvif_dm_get_wsdl_url(struct soap* soap);
const char * onvif_dm_get_service_url(struct soap* soap);
const char * onvif_dm_get_service_namespace(struct soap* soap);
void onvif_dm_get_service_version(struct soap* soap, int* major, int * minor);

const char* onvif_dm_get_manufacturer(struct soap* soap);
const char* onvif_dm_get_model(struct soap* soap);
const char* onvif_dm_get_firmware_version(struct soap* soap);
const char* onvif_dm_get_serial_number(struct soap* soap);
const char* onvif_dm_get_hardware_id(struct soap* soap);


void onvif_dm_set_system_date_time(struct soap* soap, int is_ntp, struct tm* now);
void onvif_dm_get_system_date_time(struct soap* soap, int* is_ntp, struct tm* now);

void onvif_dm_set_system_factory_default(struct soap* soap, int type);
void onvif_dm_reboot_system(struct soap* soap);

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

#endif

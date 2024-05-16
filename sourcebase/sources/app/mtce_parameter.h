/**
****************************************************************************************
* @author: PND
* @ Modified by: Your name
* @ Modified time: 2023-05-16 14:54:16
* @brief: mtce
*****************************************************************************************
**/
#ifndef __MTCE_PARAMETER_H__
#define __MTCE_PARAMETER_H__

#include <stdbool.h>

#include <vvtk_def.h>
#include <vvtk_video.h>
#include <vvtk_audio.h>

/******************************************************************************
 * Common define
 *******************************************************************************/
#define MTCE_NAME_LEN		   64
#define MTCE_NAME_PASSWORD_LEN 64
#define MTCE_CLIENT_ID_LEN	   64

/******************************************************************************
 * OSD, motion, video define
 *******************************************************************************/
#define MTCE_MAX_BITRATE	 10 * 1024
#define MTCE_MAX_FPS		 30
#define MTCE_MAX_QUALITY	 50
#define MTCE_MAX_GOP		 100
#define MTCE_SNAP_QUALITY	 60
#define MTCE_MOTION_INTERVAL 500	//(ms)

#define MTCE_JPEG_MOTION_TMP_PATH "/tmp/jpeg"
#define MTCE_MAX_SENSITIVE		  7
#define MTCE_MAX_LENG_GUARDZONE	  64

#define MTCE_OSD_FONT_PATH_FILE "/conf/user/NimbusMono-Regular.otf"

#define MTCE_MAIN_STREAM	0
#define MTCE_SUB_STREAM		1
#define MTCE_MAX_STREAM_NUM 2

#ifdef BUILD_IQ3
#define MTCE_ISP_BRIGHTNESS -35
#define MTCE_ISP_CONTRAST	40
#define MTCE_ISP_SATURATION 40
#define MTCE_ISP_SHARPNESS	60
#endif

#ifdef BUILD_PLAY3
#define MTCE_ISP_BRIGHTNESS -40
#define MTCE_ISP_CONTRAST	40
#define MTCE_ISP_SATURATION 45
#define MTCE_ISP_SHARPNESS	50
#endif

enum mtce_dayNightMode_e {
	MTCE_FULL_COLOR_MODE,	   // Off
	MTCE_NIGHT_VISION_AUTO,	   // Auto
	MTCE_BLACK_WHITE_MODE	   // On
};

/*Capture compression mode*/
enum mtce_captureComp_e {
	MTCE_CAPTURE_COMP_H264,	   // H.264
	MTCE_CAPTURE_COMP_H265,	   // H.265
	MTCE_CAPTURE_COMP_NONE	   //
};

/*capture size image*/
enum mtce_captureSize_e {
	MTCE_CAPTURE_SIZE_VGA,		// 640*480(PAL)		640*480(NTSC)
	MTCE_CAPTURE_SIZE_720P,		// 1280*720
	MTCE_CAPTURE_SIZE_1080P,	// 1920*1080
	MTCE_CAPTURE_SIZE_2K,		// 2560*1440
	MTCE_CAPTURE_SIZE_NONE,
};

/*BitRate Control*/
enum mtce_captureBrc_e {
	MTCE_CAPTURE_BRC_CBR,
	MTCE_CAPTURE_BRC_VBR,
	MTCE_CAPTURE_BRC_MBR,
	MTCE_CAPTURE_BRC_NR
};

enum mtce_captureQuality_e {
	MTCE_CAPTURE_QUALITY_LOWER = 0,
	MTCE_CAPTURE_QUALITY_LOW,
	MTCE_CAPTURE_QUALITY_NORMAL,
	MTCE_CAPTURE_QUALITY_GOOD,
	MTCE_CAPTURE_QUALITY_VERY_GOOD,
	MTCE_CAPTURE_QUALITY_BETTER,
	MTCE_CAPTURE_QUALITY_EXCELLENT,
	MTCE_CAPTURE_QUALITY_HIGH,
	MTCE_CAPTURE_QUALITY_VERY_HIGH,
	MTCE_CAPTURE_QUALITY_NONE,
};

/*Light control*/
enum mtce_lightCtrl_e {
	MTCE_LIGHT_FULL_COLOR,	   // On
	MTCE_LIGHT_BLACK_WHITE,	   // Off
	MTCE_LIGHT_AUTO			   // Auto
};

/*Light control*/
enum mtce_antiFlicker_e {
	MTCE_ANTI_FLICKER_MODE_AUTO,
	MTCE_ANTI_FLICKER_MODE_50_MHZ,
	MTCE_ANTI_FLICKER_MODE_60_MHZ
};

/*Video Format*/
typedef struct {
	int compression;	   // enum mtce_CAPTURE_COMP_E
	int resolution;		   // enum mtce_CAPTURE_SIZE_E
	int bitRateControl;	   // enum mtce_CAPTURE_BRC_E
	int quality;		   // 15 - 51
	int FPS;			   // 6 - 25/30
	int bitRate;		   // 250 - 4096
	int virtualGOP;
	int GOP;	// 2 - 12
} mtce_videoFormat_t;

/*Media format*/
typedef struct {
	mtce_videoFormat_t format;	  //
	bool bVideoEnable;			  //
	bool bAudioEnable;			  //
} mtce_mediaFormat_t;

/*Config encode simplify*/
typedef struct {
	mtce_mediaFormat_t mainFmt;		// main format for channel 0
	mtce_mediaFormat_t extraFmt;	// extra format for channel 1
} mtce_encode_t;

/*Night Vision*/
typedef struct {
	int mode;
	bool ircutSwap;
} mtce_nightVison_t;

/*Flip Mirror*/
typedef struct {
	bool picFlip;
	bool picMirror;
} mtce_flipMirror_t;

/*Camera Param*/
typedef struct {
	int nightVisionMode;
	bool pictureFlip;
	bool pictureMirror;
	bool ircutSwap;
	int antiFlickerMode;
	int lightingMode;
	bool lightingEnable;
} mtce_cameraParam_t;

/*Motion control*/
typedef struct {
	bool setSound;
	bool setLighting;
} mtce_alarmControl_t;

 

/* OSD Setting*/
#define WATERMARK_LENGTH_MAX (32)
typedef struct {
	char name[WATERMARK_LENGTH_MAX];
	bool timeAtt;
	bool nameAtt;
} mtce_watermark_t;

/******************************************************************************
 * Record define
 *******************************************************************************/
#define MOUNT_FOLDER		 "/tmp/sd/" /**Default mountpoint */
#define MTCE_AUDIO_G711A_EXT ".g711a"
#define MTCE_VIDEO_H264_EXT	 ".h264"
#define MTCE_VIDEO_H265_EXT	 ".h265"
#define MTCE_SD_CAPACITY_USE 0.85	 // 85% SD capacity used

typedef struct {
	int channel;
	int mode;
} mtce_record_t;

enum mtce_mode_e {
	MTCE_MODE_FULL,
	MTCE_MODE_MOTION,
	MTCE_MODE_MAX
};

/******************************************************************************
 * Config define
 *******************************************************************************/
#define MTCE_CONFIG_NONE 0

/*Account */
typedef struct {
	char username[32];
	char password[32];
	bool reserved;
	bool shareable;
	int len;
	char authoList[2][32];
} mtce_account_t;

/*Reset Setting*/
typedef struct {
	bool bWatermark;
	bool bMotion;
	bool bMQTT;
	bool bRTMP;
	bool bWifi;
	bool bParam;
	bool bEncode;
	bool bS3;
	bool bLedIndi;
	bool bRainbow;
} mtce_resetConfig_t;

/*Storage*/
typedef struct {
	bool bIsFornat;
} mtce_storage_t;

/*******************************************************************************
 * Camera Info Define
 ********************************************************************************/
typedef struct {
	char serial[64];
} mtce_deviceInfo_t;

typedef struct {
	char buildTime[32];
	char softVersion[16];
} mtce_sysInfo_t;

extern mtce_deviceInfo_t mtce_deviceInfo;

extern int mtce_contextInit();

/*******************************************************************************
 * MQTT Define
 ********************************************************************************/
#define MAX_MQTT_MSG_LENGTH 150000	  // Byte

enum {
	MTCE_MQTT_RESPONE_SUCCESS = 100,
	MTCE_MQTT_RESPONE_FAILED  = 102,
	MTCE_MQTT_RESPONE_TIMEOUT = 101,
};

typedef struct {
	int bEnable; /* 1: Enable; 0: Disable */
	int bTLSEnable;
	char clientID[MTCE_CLIENT_ID_LEN];
	char username[MTCE_NAME_LEN];
	char password[MTCE_NAME_PASSWORD_LEN];
	char host[MTCE_NAME_LEN];
	char TLSVersion[8];
	int keepAlive;
	int QOS;
	int retain;
	int port;
	char protocol[16];	  // IPCP_MQTT_PROTOCOL_TYPE_E
} mtce_netMQTT_t;

typedef struct {
	char topicStatus[128];
	char topicRequest[128];
	char topicResponse[128];
	char topicSignalingRequest[128];
	char topicSignalingResponse[128];
	char topicAlarm[128];
} mqttTopicCfg_t;

/*******************************************************************************
 * task fw Define
 ********************************************************************************/

typedef struct {
	char fileName[16];
	char putUrl[128];
} mtce_upload_file_t;

/*******************************************************************************
 * task network Define
 ********************************************************************************/
enum mtce_net_status_e {
	MTCE_NET_UNKNOWN,
	MTCE_NET_CONNECTED,
	MTCE_NET_DISCONNECT,
};

typedef struct {
	bool enable;
	char ssid[64];
	char keys[64];
	int keyType;
	char auth[32];
	int channel;
	char encrypType[16];
	char gateWay[16];
	char hostIP[16];
	char submask[16];
	bool dhcp;
} mtce_netWifi_t;

typedef struct {
	char mac[32];
	char hostIP[16];
	char subnet[16];
	char gateway[16];
	char dns[16];
} mtce_netIPAddress_t;

#define MTCE_WIFI_SCAN_HOST_AP_NUM_MAX 10
typedef struct {
	char mac[20];
	char flag[128];
	char ssid[64];
	int signal;
	int freq;
} netItem_t;

typedef struct {
	int number;
	netItem_t item[MTCE_WIFI_SCAN_HOST_AP_NUM_MAX];
} mtce_netWifiStaScan_t;
/*******************************************************************************
 * task upload Define
 ********************************************************************************/
/* S3 Setting*/
typedef struct {
	char accessKey[256];
	char endPoint[128];
} mtce_s3_t;

/*******************************************************************************
 * task RTMP Define
 ********************************************************************************/
/* RTMP Setting*/
typedef struct {
	char host[64];
	bool enable;
	int port;
	char token[64];
	char node[64];
	int streamType;
} mtce_rtmp_t;

/*******************************************************************************
 * PTZ Define
 ********************************************************************************/
/* PTZ*/
typedef struct {
	char direction[16];
	int step;
	int x;
	int y;
} mtce_ptz_t;

#endif	  // __MTCE_PARAMETER_H__

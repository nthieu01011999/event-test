#ifndef __APP_H__
#define __APP_H__

#include <string>

#include "app_config.h"
#include "cpptime.h"

using namespace std;

#define MTCE_SERIAL_NUM_DEFAULT		"24010000004"


/*****************************************************************************/
/* task GW_AV define
 */
/*****************************************************************************/
/* define timer */
#define GW_AV_CHECK_CAPTURE_PROCESS_TIMEOUT_INTERVAL (10000)
#define GW_AV_AUTO_CONTROL_DAYNIGHT_TIMEOUT_INTERVAL (5000)
#define GW_AV_VIDEO_SET_ENCODE_TIMEOUT_INTERVAL		 (5000)
#define GW_AV_OSD_UPDATE_TIME_TEXT_TIMEOUT_INTERVAL	 (1000)

/* define signal */

enum {
	GW_AV_WATCHDOG_PING_REQ = AK_USER_WATCHDOG_SIG,
	GW_AV_INIT_REQ			= AK_USER_DEFINE_SIG,
	GW_AV_SET_CAMERAPARAM_REQ,
	GW_AV_GET_CAMERAPARAM_REQ,
	GW_AV_SET_ENCODE_REQ,
	GW_AV_SET_ENCODE_REQ_TO,
	GW_AV_APPLY_NEW_ENCODE_REQ,
	GW_AV_GET_ENCODE_REQ,
	GW_AV_SET_WATERMARK_REQ,
	GW_AV_GET_WATERMARK_REQ,
	GW_AV_CHECK_CAPTURE_RUNNING_REQ,
	GW_AV_CHECK_VIDEO_STREAM_RUNNING_REQ,
	GW_AV_RELOAD_PICTURE_REQ,
	GW_AV_SET_ENCODE_STOP_VIDEO_MAIN_STREAM_RES,
	GW_AV_SET_ENCODE_STOP_VIDEO_SUB_STREAM_RES,
	GW_AV_SET_ENCODE_STOP_RTC_AUDIO_STREAM_RES,
	GW_AV_SET_ENCODE_STOP_RTMP_AUDIO_STREAM_RES,
};

/*****************************************************************************/
/*  task GW_TASK_WEBRTC define
 */
/*****************************************************************************/
/* define timer */
#define GW_WEBRTC_ERASE_CLIENT_NO_ANSWER_TIMEOUT_INTERVAL (40000) /* 40s */
#define GW_WEBRTC_TRY_CONNECT_SOCKET_INTERVAL			  (10000) /* 10s */
#define GW_WEBRTC_TRY_GET_EXTERNAL_IP_INTERVAL			  (7000)  /* 7s */
#define GW_WEBRTC_WAIT_REQUEST_TIMEOUT_INTERVAL			  (20000) /* 20s */
#define GW_WEBRTC_CLIENT_SEND_PING_INTERVAL				  (10000) /* 10s */
#define GW_WEBRTC_ERASE_CLIENT_PING_PONG_TIMEOUT_INTERVAL (20000) /* 20s */
#define GW_WEBRTC_RELEASE_CLIENT_PUSH_TO_TALK_INTERVAL	  (2500)  /* 2.5s */

/* define signal */
enum {
	GW_WEBRTC_WATCHDOG_PING_REQ	 = AK_USER_WATCHDOG_SIG,
	GW_WEBRTC_SIGNALING_MQTT_REQ = AK_USER_DEFINE_SIG,
#ifdef TEST_USE_WEB_SOCKET
	GW_WEBRTC_TRY_CONNECT_SOCKET_REQ,
	GW_WEBRTC_SIGNALING_SOCKET_REQ,
#endif
	GW_WEBRTC_SET_STUN_SERVER_REQ,
	GW_WEBRTC_GET_STUN_SERVER_REQ,
	GW_WEBRTC_SET_TURN_SERVER_REQ,
	GW_WEBRTC_GET_TURN_SERVER_REQ,
	GW_WEBRTC_CHECK_CLIENT_CONNECTED_REQ,
	GW_WEBRTC_ERASE_CLIENT_REQ,
	GW_WEBRTC_DBG_IPC_SEND_MESSAGE_REQ,
	GW_WEBRTC_ON_MESSAGE_CONTROL_DATACHANNEL_REQ,
	GW_WEBRTC_DATACHANNEL_DOWNLOAD_RELEASE_REQ,
	GW_WEBRTC_RELEASE_CLIENT_PUSH_TO_TALK,

	GW_HELLO_PRINT,
	GW_WEBRTC_ICE_CANDIDATE,
};

/*****************************************************************************/
/*  task GW_CLOUD define
 */
/*****************************************************************************/
/* define timer */
#define GW_WEB_MQTT_TRY_CONNECT_TIMEOUT_INTERVAL		  (5000)
#define GW_WEB_MQTT_CHECK_CONNECT_STATUS_INTERVAL		  (10000)
#define GW_WEB_MQTT_TRY_CONNECT_AFTER_DISCONNECT_INTERVAL (20000)

/* define signal */
enum {
	GW_CLOUD_WATCHDOG_PING_REQ = AK_USER_WATCHDOG_SIG,
	GW_CLOUD_MQTT_INIT_REQ	   = AK_USER_DEFINE_SIG,
	GW_CLOUD_MQTT_TRY_CONNECT_REQ,
	GW_CLOUD_MQTT_CHECK_CONNECT_STATUS_REQ,
	GW_CLOUD_DATA_COMMUNICATION,
	GW_CLOUD_SET_MQTT_CONFIG_REQ,
	GW_CLOUD_GET_MQTT_CONFIG_REQ,
	GW_CLOUD_CAMERA_STATUS_RES,
	GW_CLOUD_CAMERA_CONFIG_RES,
	GW_CLOUD_CAMERA_ALARM_RES,
	GW_CLOUD_SIGNALING_MQTT_RES,
	GW_CLOUD_MESSAGE_LENGTH_OUT_OF_RANGE_REP,
};

/*****************************************************************************/
/*  global define variable
 */
/*****************************************************************************/
#define APP_OK (0x00)
#define APP_NG (0x01)

#define APP_FLAG_OFF (0x00)
#define APP_FLAG_ON	 (0x01)

/* define file name */
#define MTCE_SERIAL_FILE			"Serial"
#define MTCE_ACCOUNT_FILE			"Account"
#define MTCE_P2P_FILE				"P2P"
#define MTCE_ENCODE_FILE			"Encode"
#define MTCE_DEVINFO_FILE			"DeviceInfo"
#define MTCE_MOTION_FILE			"Motion"
#define MTCE_WIFI_FILE				"Wifi"
#define MTCE_WATERMARK_FILE			"Watermark"
#define MTCE_S3_FILE				"S3"
#define MTCE_RTMP_FILE				"RTMP"
#define MTCE_NETWORK_CA_FILE		"Bundle_RapidSSL_2023.cert"
#define MTCE_NETWORK_MQTT_FILE		"MQTTService"
#define MTCE_NETWORK_WPA_FILE		"wpa_supplicant.conf"
#define MTCE_CAMERA_PARAM_FILE		"CameraParam"
#define MTCE_PTZ_FILE				"PTZ"
#define MTCE_LOG_FILE				"app.log"
#define MTCE_RTC_SERVERS_FILE		"RtcServersConfig"
#define MTCE_STORAGE_FILE			"Storage"
#define MTCE_RECORD_FILE			"Record"
#define MTCE_CHECKSUM_FILE			"SdChecksumFile"
#define MTCE_OTA_STATUS				"OTAStatus"
#define MTCE_IO_DRIVER_CONTROL_FILE "IoControl"
#define MTCE_RAINBOW_FILE			"rainbow.json"
#define MTCE_OWNER_STATUS			"OwnerStatus"
#define MTCE_ALARM_CONTROL_FILE		"AlarmControl"
#define MTCE_STEP_MOTOR_FILE		"StepMotors"
#define MTCE_SERIAL_NUM_DEFAULT		"24010000004"
#define MTCE_MAC_DEFAULT			"00:E0:4C:35:00:00"

/* define path name */
#define MTCE_DEVICE_SOUND_PATH		   "/app/sound"
#define MTCE_DEVICE_INFO_PATH		   "/app/version"
#define MTCE_USER_CONF_PATH			   "/usr/conf"
#define MTCE_DFAUL_CONF_PATH		   "/app/default"
#define MTCE_LOG_FILE_PATH			   MTCE_USER_CONF_PATH "/log"
#define MTCE_MEDIA_JPEG_PATH		   "/tmp/jpeg"
#define MTCE_MEDIA_MP4_PATH			   "/tmp/mp4"
#define MTCE_SDCARD_CHECKSUM_FILE_PATH "/tmp/sdFileInfo"

/* use for wifi and network */
#define MTCE_NET_WIRED_IF_NAME		  "eth0"
#define MTCE_NET_WIFI_STA_IF_NAME	  "wlan0"
#define MTCE_NET_WIFI_AP_IF_NAME	  "wlan1"
#define MTCE_UDHCPC_SCRIPT			  "/app/bin/udhcpc.sh"
#define MTCE_NET_UDHCPC_PID_FILE	  "/var/run/%s.pid"
#define MTCE_NET_WIFI_AP_UDHCPD_FILE  "/tmp/udhcpd.conf"
#define MTCE_NET_WIFI_AP_HOSTADP_FILE "/tmp/hostapd.conf"
#define MTCE_NET_WIFI_STA_WPA_FILE	  "/tmp/wpa_supplicant.conf"

#define MTCE_AUDIO_AAC_EXT	".aac"
#define MTCE_AUDIO_G711_EXT ".g711"

#define MTCE_MQTT_KEEPALIVE	 90
#define MTCE_SSL_VERIFY_NONE 0
#define MTCE_SSL_VERIFY_PEER 1

#define MTCE_SOUND_REBOOT_DEVICE_FILE		"reboot_device.g711a"
#define MTCE_SOUND_WAIT_CONNECT_FILE		"waiting_for_connection.g711a"
#define MTCE_SOUND_HELLO_DEVICE_FILE		"hello_fpt_camera.g711a"
#define MTCE_SOUND_NET_CONNECT_SUCCESS_FILE "connection_success.g711a"
#define MTCE_SOUND_MOTION_ALARM_FILE		"motion_alarm.g711a"
extern CppTime::Timer systemTimer;

#endif	  // __APP_H__

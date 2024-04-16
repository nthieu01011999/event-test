#ifndef __APP_H__
#define __APP_H__

#include <string>

#include "app_config.h"
#include "cpptime.h"

using namespace std;

#define MTCE_SERIAL_NUM_DEFAULT		"24010000004"
/*****************************************************************************/
/*  task GW_TASK_WEBRTC define
 */
/*****************************************************************************/
/* define timer */
#define GW_WEBRTC_ERASE_CLIENT_NO_ANSWER_TIMEOUT_INTERVAL (40000) /* 40s */
#define GW_WEBRTC_TRY_CONNECT_SOCKET_INTERVAL			  (10000) /* 10s */
#define GW_WEBRTC_WAIT_REQUEST_TIMEOUT_INTERVAL			  (20000) /* 20s */
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
};

extern CppTime::Timer systemTimer;

#endif	  // __APP_H__

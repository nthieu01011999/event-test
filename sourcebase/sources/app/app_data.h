#ifndef __APP_DATA_H__
#define __APP_DATA_H__

#include <stdint.h>
#include <stdbool.h>
#include <iostream>
#include <vector>
#include <string>
#include "ak.h"
#include "mtce_parameter.h"

#define TEST_USE_WEB_SOCKET 1
#define CHECK_TIME_EXE		0

typedef struct {
	char clientId[30];
	char msg[200];
} DataChannelSend_t;

typedef struct {
	std::string wSocketServerCfg; // store urls signaling
	std::vector<std::string> arrStunServerUrl;
	std::vector<std::string> arrTurnServerUrl;

	void clear() {
		wSocketServerCfg.clear();
		arrStunServerUrl.clear();
		arrTurnServerUrl.clear();
	}
} rtcServersConfig_t;

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
};

extern std::string mtce_getSerialInfo();

#ifdef __cplusplus
extern "C" {
#endif

// extern void onSampleVideoCapture(int channel, uint8_t *bytes, uint32_t nbBytes);
// extern void onSampleAudioALawCapture(uint8_t *bytes, uint32_t nbBytes);
// extern void onSampleAudioAACCapture(uint8_t *bytes, uint32_t nbBytes);

#ifdef __cplusplus
}
#endif

#endif /* __APP_DATA_H__ */
#ifndef __APP_DATA_H__
#define __APP_DATA_H__

#include <stdint.h>
#include <stdbool.h>
#include <iostream>
#include <vector>
#include <string>
#include "ak.h"
#include "mtce_parameter.h"
#include "helpers.hpp"
#define TEST_USE_WEB_SOCKET 1
#define CHECK_TIME_EXE		0

extern ClientsGroup_t clients;

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

extern void lockMutexListClients();
extern void unlockMutexListClients();

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
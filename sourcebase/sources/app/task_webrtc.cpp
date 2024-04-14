#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <vector>

#include <algorithm>
#include <future>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <atomic>

#include "app.h"
#include "app_data.h"
#include "app_config.h"
#include "app_dbg.h"
#include "rtc/rtc.hpp"
#include "json.hpp"

#ifdef BUILD_ARM_VVTK
#endif

#include "parser_json.h"
#include "utils.h"
#include "task_list.h"


#define CLIENT_SIGNALING_MAX 20
#define CLIENT_MAX 10

using namespace rtc;
using namespace std;
using namespace chrono_literals;
using namespace chrono;
using json = nlohmann::json;

q_msg_t gw_task_webrtc_mailbox;
static Configuration rtcConfig;

static int8_t loadIceServersConfigFile(Configuration &rtcConfig);
static int8_t loadWsocketSignalingServerConfigFile(string &wsUrl);

void *gw_task_webrtc_entry(void *) {
	
    // wait_all_tasks_started();
	APP_DBG("[STARTED_1] gw_task_webrtc_entry\n");
    if (loadIceServersConfigFile(rtcConfig) != APP_CONFIG_SUCCESS) {
        APP_PRINT("Failed to load ICE servers configuration.\n");
        return nullptr;  // Exit if configuration fails
    }

    rtcConfig.disableAutoNegotiation = false;  // Set localDescription automatically

    // WebSocket initialization
    auto ws = make_shared<WebSocket>();
    atomic<bool> isConnected(false);

    ws->onOpen([&]() {
        isConnected.store(true);
        APP_PRINT("WebSocket connected, signaling ready\n");
    });

    ws->onClosed([&]() {
        isConnected.store(false);
        APP_PRINT("WebSocket closed\n");
    });

    ws->onError([&](const string &error) {
        isConnected.store(false);
        APP_PRINT("WebSocket connection failed: %s\n", error.c_str());
    });

    ws->onMessage([&](variant<binary, string> data) {
        if (holds_alternative<string>(data)) {
            string msg = get<string>(data);
            APP_DBG("%s\n", msg.data());
            // Handle signaling message
        }
    });

    std::string wsUrl;
    if (loadWsocketSignalingServerConfigFile(wsUrl) != APP_CONFIG_SUCCESS || wsUrl.empty()) {
        APP_PRINT("Failed to load WebSocket URL configuration or URL is empty.\n");
        return nullptr;  // Exit if configuration fails or URL is empty
    }

    if (wsUrl.empty() || wsUrl.find("ws://") != 0) {
        APP_PRINT("Invalid WebSocket URL provided: %s\n", wsUrl.c_str());
        return nullptr;
    }

    try {
        ws->open(wsUrl);
        this_thread::sleep_for(1s);  // Wait for connection to establish

        if (!isConnected.load()) {
            APP_PRINT("WebSocket connection failed to establish.\n");
            return nullptr;  // Exit if WebSocket fails to connect
        }
    } catch (const std::exception& e) {
        APP_PRINT("Exception caught while initializing WebSocket: %s\n", e.what());
        return nullptr;  // Exit if an exception occurred during WebSocket setup
    }

    APP_DBG("[STARTED] gw_task_webrtc_entry\n");

    ak_msg_t *msg = nullptr;
	while (true) {
		msg = ak_msg_rev(GW_TASK_WEBRTC_ID);

		if (msg != nullptr) {
			// Process the message
			// Make sure to handle msg content safely and properly here.

			ak_msg_free(msg);
		} else {
			// No message was received, sleep to prevent busy waiting.
			this_thread::sleep_for(100ms);
		}
	}

    return nullptr;
}

int8_t loadIceServersConfigFile(Configuration &rtcConfig) {
    rtcServersConfig_t rtcServerCfg;
    int8_t ret = configGetRtcServers(&rtcServerCfg);
    if (ret == APP_CONFIG_SUCCESS) {
        rtcConfig.iceServers.clear();
        for (const auto& url : rtcServerCfg.arrStunServerUrl) {
            if (!url.empty()) {
                rtcConfig.iceServers.emplace_back(url);
            }
        }
        for (const auto& url : rtcServerCfg.arrTurnServerUrl) {
            if (!url.empty()) {
                rtcConfig.iceServers.emplace_back(url);
            }
        }
    }
    return ret;
}

int8_t loadWsocketSignalingServerConfigFile(string &wsUrl) {
    rtcServersConfig_t rtcServerCfg;
    int8_t ret = configGetRtcServers(&rtcServerCfg);
    if (ret == APP_CONFIG_SUCCESS && !rtcServerCfg.wSocketServerCfg.empty()) {
        wsUrl = rtcServerCfg.wSocketServerCfg + "/" + mtce_getSerialInfo();
    }
    return ret;
}
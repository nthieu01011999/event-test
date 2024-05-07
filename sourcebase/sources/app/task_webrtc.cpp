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
#include "datachannel_hdl.h"

#include "task_list.h"

// #include "dispatchqueue.hpp"
#include "helpers.hpp"
#include "rtc/rtc.hpp"
#include "json.hpp"
// #include "stream.hpp"

#ifdef BUILD_ARM_VVTK
// #include "h26xsource.hpp"
// #include "audiosource.hpp"
#endif

// #include "mtce_audio.hpp"
#include "parser_json.h"
#include "utils.h"

#define CLIENT_SIGNALING_MAX 20
#define CLIENT_MAX			 10

using namespace rtc;
using namespace std;
using namespace chrono_literals;
using namespace chrono;
using json = nlohmann::json;

q_msg_t gw_task_webrtc_mailbox;
std::shared_ptr<PeerConnection> pc = nullptr;  // AK_MSG_NULL has been did with #define AK_MSG_NULL ((ak_msg_t *)0)
std::shared_ptr<WebSocket> globalWebSocket;
shared_ptr<Client> createPeerConnection(const Configuration &rtcConfig, weak_ptr<WebSocket> wws, const string& id);

static int8_t loadWsocketSignalingServerConfigFile(string &wsUrl);
static Configuration rtcConfig;
static int8_t loadIceServersConfigFile(Configuration &rtcConfig);
void handleWebSocketMessage(const std::string& message, std::shared_ptr<WebSocket> ws);
void handleClientRequest(const std::string& clientId, std::shared_ptr<WebSocket> ws);
void handleAnswer(const std::string& id, const json& message);

std::unordered_map<std::string, std::shared_ptr<PeerConnection>> peerConnections;



void *gw_task_webrtc_entry(void *) {
	ak_msg_t *msg = AK_MSG_NULL;

	wait_all_tasks_started();
	APP_DBG("[STARTED_1] gw_task_webrtc_entry\n");
    if (loadIceServersConfigFile(rtcConfig) != APP_CONFIG_SUCCESS) {
        APP_PRINT("Failed to load ICE servers configuration.\n");
        return AK_MSG_NULL;  // Exit if configuration fails
    }

    rtcConfig.disableAutoNegotiation = false;  // Set localDescription automatically
	
    /* init websocket */
    auto ws = make_shared<WebSocket>(); // init poll service and threadpool = 4
	weak_ptr<WebSocket> wws = ws;
    // Guard flag to check WebSocket connection status
    atomic<bool> isConnected(false);

    ws->onOpen([&]() {
        isConnected.store(true);
        APP_PRINT("WebSocket connected, signaling ready\n");
        timer_remove_attr(GW_TASK_WEBRTC_ID, GW_WEBRTC_TRY_CONNECT_SOCKET_REQ);
    });

    ws->onClosed([&]() {
        isConnected.store(false);
        APP_PRINT("WebSocket closed\n");
        timer_set(GW_TASK_WEBRTC_ID, GW_WEBRTC_TRY_CONNECT_SOCKET_REQ, GW_WEBRTC_TRY_CONNECT_SOCKET_INTERVAL, TIMER_ONE_SHOT);
    });

    ws->onError([&](const string &error) {
        isConnected.store(false);
        APP_PRINT("WebSocket connection failed: %s\n", error.c_str());
        // Depending on your application logic, you might want to attempt a reconnection here.
    });

	ws->onMessage([&](variant<binary, string> data) {
		APP_PRINT("WebSocket connection @ ws->onMessage\n");
		if (holds_alternative<string>(data)) {
			string msg = get<string>(data);
			APP_DBG("%s\n", msg.data());
			handleWebSocketMessage(msg, ws);  // Pass the message to the handler
			// If needed, you can post the message here as well, depending on your application's architecture.
		}
	});

    std::string wsUrl;


    // Debugging output
    wsUrl = "ws://sig.espitek.com:8089/" + mtce_getSerialInfo();
    std::cout << "Attempting to connect WebSocket server at URL: " << wsUrl << std::endl;

    if (loadWsocketSignalingServerConfigFile(wsUrl) != APP_CONFIG_SUCCESS || wsUrl.empty()) {
        APP_PRINT("Failed to load WebSocket URL configuration or URL is empty.\n");
        return AK_MSG_NULL;  // Exit if configuration fails or URL is empty
    }
    // Attempt to connect
    ws->open(wsUrl);

    // Wait a bit to see if the connection succeeds (or modify based on your app logic)
    std::this_thread::sleep_for(1s); // Adjust the timing as necessary

    // Check the connection status
    if (isConnected.load()) {
        APP_PRINT("WebSocket is successfully connected.\n");
    } else {
        APP_PRINT("WebSocket connection failed or is still in progress.\n");
    }

// #endif
	APP_DBG("[STARTED] gw_task_webrtc_entry\n");

	while (1) {
		/* get messge */
		msg = ak_msg_rev(GW_TASK_WEBRTC_ID);

		switch (msg->header->sig) {
        case GW_WEBRTC_ERASE_CLIENT_REQ: {
            APP_DBG_SIG("GW_WEBRTC_ERASE_CLIENT_REQ\n");
            string id((char *)msg->header->payload);
            APP_PRINT("clear client id: %s\n", id.c_str());
            Client::setSignalingStatus(true);
            lockMutexListClients();
            clients.erase(id);
            unlockMutexListClients();
            Client::setSignalingStatus(false);

            // printAllClients();
		} break;

		case GW_WEBRTC_ICE_CANDIDATE: {

		} break;

		case GW_WEBRTC_ON_MESSAGE_CONTROL_DATACHANNEL_REQ: {
			APP_DBG_SIG("GW_WEBRTC_ON_MESSAGE_CONTROL_DATACHANNEL_REQ\n");
			try {
				json message = json::parse(string((char *)msg->header->payload, msg->header->len));
				string id	 = message["ClientId"].get<string>();
				string data	 = message["Data"].get<string>();
				string resp	 = "";
				APP_DBG("client id: %s, msg: %s\n", id.c_str(), data.c_str());
				onDataChannelHdl(id, data, resp);
				sendMsgControlDataChannel(id, resp);
			}
			catch (const exception &error) {
				APP_DBG("%s\n", error.what());
			}
			APP_DBG("end datachannel handle\n");
		} break;
			
		default:
		break;
		}

		/* free message */
		ak_msg_free(msg);
	}

    // rtcConfig.disableAutoNegotiation = false;
    pc = make_shared<PeerConnection>(rtcConfig);

	return (void *)0;
}

int8_t loadIceServersConfigFile(Configuration &rtcConfig) {
	rtcServersConfig_t rtcServerCfg;
	int8_t ret = configGetRtcServers(&rtcServerCfg);
	try {
		if (ret == APP_CONFIG_SUCCESS) {
			rtcConfig.iceServers.clear();

			APP_DBG("List stun server:\n");
			string url = "";
			int size   = rtcServerCfg.arrStunServerUrl.size();
			for (int idx = 0; idx < size; idx++) {
				url = rtcServerCfg.arrStunServerUrl.at(idx);
				APP_DBG("\t[%d] url: %s\n", idx + 1, url.c_str());
				if (url != "") {
					rtcConfig.iceServers.emplace_back(url);
				}
			}
			APP_DBG("\n");
			APP_DBG("List turn server:\n");
			size = rtcServerCfg.arrTurnServerUrl.size();
			for (int idx = 0; idx < size; idx++) {
				url = rtcServerCfg.arrTurnServerUrl.at(idx);
				APP_DBG("\t[%d] url: %s\n", idx + 1, url.c_str());
				if (url != "") {
					rtcConfig.iceServers.emplace_back(url);
				}
			}
			APP_DBG("\n");
		}
	}
	catch (const exception &error) {
		APP_DBG("loadIceServersConfigFile %s\n", error.what());
		ret = APP_CONFIG_ERROR_DATA_INVALID;
	}

	return ret;
}


int8_t loadWsocketSignalingServerConfigFile(string &wsUrl) {
    rtcServersConfig_t rtcServerCfg;
    int8_t ret = configGetRtcServers(&rtcServerCfg);
    
    APP_PRINT("[DEBUG] Loading WebSocket Signaling Server Config...\n");
    
    if (ret == APP_CONFIG_SUCCESS) {
        APP_PRINT("[DEBUG] Successfully retrieved RTC server configuration.\n");
        
        wsUrl.clear();
        // if (!rtcServerCfg.wSocketServerCfg.empty()) {
		if (rtcServerCfg.wSocketServerCfg 	!= "") {
            wsUrl = rtcServerCfg.wSocketServerCfg + "/" + mtce_getSerialInfo();
            APP_PRINT("[DEBUG] WebSocket URL constructed: %s\n", wsUrl.c_str());
        } else {
            APP_PRINT("[ERROR] WebSocket server configuration is empty.\n");
        }
    } else {
        APP_PRINT("[ERROR] Failed to retrieve RTC server configuration. Error code: %d\n", ret);
    }
    
    return ret;
}
// /
// "Exchange of Offer and Answer"
void handleWebSocketMessage(const std::string& message, std::shared_ptr<WebSocket> ws) {

    APP_DBG("[WebSocket Message Received]: %s\n", message.c_str());
    if (ws && ws->isOpen()) {
        // It is safe to use 'ws' here
    } else {
        APP_DBG("WebSocket is not open or not available.\n");
        // Handle the error or attempt to reconnect
    }

    try {
        json messageJson = json::parse(message);
        APP_DBG("Parsed WebSocket Message: %s\n", messageJson.dump(4).c_str()); // Use pretty printing for better readability

        auto typeIt = messageJson.find("Type");
        if (typeIt != messageJson.end()) {
            string type = typeIt->get<string>();
            APP_DBG("Processing Message Type: %s\n", type.c_str());

            if (type == "request") {
                auto clientIdIt = messageJson.find("ClientId");
                if (clientIdIt != messageJson.end()) {
                    string clientId = clientIdIt->get<string>();
                    APP_DBG("Handling request for Client ID: %s\n", clientId.c_str());

                    // Handle the client's request, e.g., create a new peer connection
                    handleClientRequest(clientId, ws);
                } else {
                    APP_DBG("Error: ClientId not found in request message\n");
                }
            } else if (type == "candidate") {
				APP_DBG("[Type: candidate]\n");
                string sdp = messageJson["sdp"].get<string>();
                string mid = messageJson["mid"].get<string>();
                APP_DBG("Adding ICE Candidate: sdp=%s, mid=%s\n", sdp.c_str(), mid.c_str());
                
                // pc->addRemoteCandidate(Candidate(sdp, mid));
            } 
			else if (type == "answer") {
				auto clientIdIt = messageJson.find("ClientId");
				if (clientIdIt != messageJson.end()) {
					string clientId = clientIdIt->get<string>();
					APP_DBG("Received answer for client ID: %s\n", clientId.c_str());
					auto pcIter = peerConnections.find(clientId);
					if (pcIter != peerConnections.end()) {
						auto& pc = pcIter->second;
						if (pc->localDescription() && pc->localDescription()->type() == Description::Type::Offer) {
							handleAnswer(clientId, messageJson);
						} else {
							APP_DBG("Received answer in an unexpected state.\n");
						}
					} else {
						APP_DBG("No PeerConnection found for client ID: %s\n", clientId.c_str());
					}
				}
			}
        } else {
            APP_DBG("Error: Message type not specified\n");
        }
    } catch (const json::exception& e) {
        APP_DBG("[JSON Parsing Error] %s\n", e.what());
    }

    // Continue to post the message to the task or message queue
    task_post_dynamic_msg(GW_TASK_WEBRTC_ID, GW_WEBRTC_SIGNALING_SOCKET_REQ, (uint8_t *)message.data(), message.length() + 1);
}

void handleClientRequest(const std::string& clientId, std::shared_ptr<WebSocket> ws) {
    APP_DBG("Initiating peer connection for Client ID: %s\n", clientId.c_str());

    // Check client limits
    if (Client::totalClientsConnectSuccess <= CLIENT_MAX && clients.size() <= CLIENT_SIGNALING_MAX) {
        Client::setSignalingStatus(true);
        APP_DBG("Client limits OK, proceeding to create peer connection\n");
		// auto ws = make_shared<WebSocket>(); // This should be managed globally or outside this function
        // weak_ptr<WebSocket> wws = ws; // Convert shared_ptr to weak_ptr
        // // Create a new peer connection for the client
        std::shared_ptr<Client> newClient = createPeerConnection(rtcConfig, make_weak_ptr(ws), clientId); //each id each peer following
		peerConnections[clientId] = newClient->peerConnection;
        lockMutexListClients();
        clients.emplace(clientId, newClient);
        // printAllClients();
        auto cl = clients.at(clientId);
        cl->startTimeoutConnect();
        APP_DBG("Started timeout connect for client: %s\n", clientId.c_str());
        unlockMutexListClients();
        Client::setSignalingStatus(false);
    } else {
        APP_DBG("Reached max client limit. Cannot handle new request for client: %s\n", clientId.c_str());
    }
}

void handleAnswer(const std::string& id, const json& message) {
    auto pcIter = peerConnections.find(id);
    if (pcIter == peerConnections.end()) {
        APP_DBG("No PeerConnection found for client ID: %s\n", id.c_str());
        return;
    }
	std::shared_ptr<PeerConnection>& pc = pcIter->second;
    if (!pc) {
        APP_DBG("PeerConnection pointer is null, cannot handle answer for client ID: %s\n", id.c_str());
        // Depending on your application logic, you may want to reconnect, throw an exception, or handle this error appropriately.
        return;
    }
    auto sdp = message["Sdp"].get<string>();
    auto desRev = Description(sdp, "answer");

    if (pc->remoteDescription().has_value()) {
        // Existing remote description, check ICE credentials
        if (desRev.iceUfrag().has_value() && desRev.icePwd().has_value() &&
            desRev.iceUfrag().value() == pc->remoteDescription().value().iceUfrag().value() &&
            desRev.icePwd().value() == pc->remoteDescription().value().icePwd().value()) {

            // Add remote candidates if any
            auto remoteCandidates = desRev.extractCandidates();
            for (const auto& candidate : remoteCandidates) {
                APP_DBG("Adding ICE Candidate: %s\n", candidate.candidate().c_str());
                pc->addRemoteCandidate(candidate);
            }
        } else {
            APP_DBG("ICE credentials mismatch for client ID: %s\n", id.c_str());
            // Handle error, possibly throw an exception
        }
    } else {
        APP_DBG("Setting new remote description for client ID: %s\n", id.c_str());
        try {
            pc->setRemoteDescription(desRev);
            APP_DBG("New session description set for client ID: %s\n", id.c_str());
        } catch (const exception& e) {
            APP_DBG("Error setting remote description for client ID: %s: %s\n", id.c_str(), e.what());
            // Handle error, possibly throw an exception
        }
    }
}


shared_ptr<Client> createPeerConnection(const Configuration &rtcConfig, weak_ptr<WebSocket> wws, const string& id) {
	if (wws.expired()) {
		APP_DBG("WebSocket pointer expired before creating PeerConnection for client ID: %s\n", id.c_str());
		// Handle the expired case (e.g., throw an exception or return a nullptr)
		return nullptr;
	}
	else
		APP_DBG("[ALIVE]\n");

	APP_DBG("call createPeerConnection()\n");
	auto pc = make_shared<PeerConnection>(rtcConfig);
	APP_DBG("PeerConnection created successfully.\n"); // Add this line to confirm creation
	auto client = make_shared<Client>(pc);
	client->setId(id);

	pc->onStateChange([id](PeerConnection::State state) {
		APP_DBG("State: %d\n", (int)state);
		if (state == PeerConnection::State::Disconnected || state == PeerConnection::State::Failed || state == PeerConnection::State::Closed) {
			// remove disconnected client
			APP_DBG("call erase client from lib\n");
			systemTimer.add(milliseconds(100),
							[id](CppTime::timer_id) { task_post_dynamic_msg(GW_TASK_WEBRTC_ID, GW_WEBRTC_ERASE_CLIENT_REQ, (uint8_t *)id.c_str(), id.length() + 1); });
		}
	});

	pc->onGatheringStateChange([wpc = make_weak_ptr(pc), id, wws](PeerConnection::GatheringState state) {
	APP_DBG("Gathering State: %d\n", static_cast<int>(state));
	if (state == PeerConnection::GatheringState::Complete) {
		auto pc = wpc.lock();
		if (!pc) {
			APP_DBG("PeerConnection weak_ptr expired for client: %s.\n", id.c_str());
			return;
		}

		auto description = pc->localDescription();
		if (!description) {
			APP_DBG("Failed to get local description for client: %s.\n", id.c_str());
			return;
		}

		json message = {
			{"ClientId", id},
			{"Type", description->typeString()},
			{"Sdp", string(description.value())}
		};

		APP_DBG("[BEFORE] Sent SDP to WebSocket: %s\n", message.dump().c_str());

		// Check if the WebSocket pointer is still valid
		if (auto ws = wws.lock()) {
			try {
				// Check if the WebSocket connection is open
				if (ws->isOpen()) { // Hypothetical function to check connection status
					APP_DBG("WebSocket is alive, sending message\n");
					ws->send(message.dump());
					APP_DBG("Sent SDP to WebSocket for client: %s.\n", id.c_str());
				} else {
					APP_DBG("WebSocket is not open for client: %s.\n", id.c_str());
					// Implement reconnection or error handling logic here
				}
			} catch (const std::exception& e) {
				APP_DBG("Exception while sending message for client: %s: %s\n", id.c_str(), e.what());
				// Implement error handling logic here
			}
		} else {
			APP_DBG("WebSocket weak_ptr expired for client: %s.\n", id.c_str());
			// Implement reconnection or error handling logic here
		}
	}
	});



	pc->onLocalCandidate([id, wpc = make_weak_ptr(pc)](const Candidate &candidate) {
		APP_DBG("Entered onLocalCandidate callback for client: %s.\n", id.c_str());
		if (wpc.expired()) {
			APP_DBG("PeerConnection weak pointer expired for client: %s.\n", id.c_str());
			return;
		}
		
		auto lockedPc = wpc.lock();
		if (!lockedPc) {
			APP_DBG("Failed to lock PeerConnection for client: %s.\n", id.c_str());
			return;
		}
		
		APP_DBG("PeerConnection locked successfully for client: %s.\n", id.c_str());
		
		json message = {
			{"Type", "candidate"},
			{"Candidate", candidate.candidate()},
			{"SdpMid", candidate.mid()},
			{"ClientId", id}
		};
		std::cout << message.dump(4) << std::endl;

		std::string message_str = message.dump();
		APP_DBG("JSON message constructed for client: %s, message: %s\n", id.c_str(), message_str.c_str());

		try {
			task_post_dynamic_msg(GW_TASK_HELLO_ID, GW_WEBRTC_ICE_CANDIDATE, (uint8_t *)message_str.data(), message_str.length() + 1);
			APP_DBG("ICE candidate message posted successfully for client: %s.\n", id.c_str());
		} catch (const std::exception& e) {
			APP_DBG("Exception caught while posting ICE candidate message for client: %s: %s\n", id.c_str(), e.what());
		}
	});


// 	client->video = addVideo(pc, 102, 1, "VideoStream", "Stream", [id, wc = make_weak_ptr(client)]() {	  // TODO add peer sergment fault
// 		if (auto c = wc.lock()) {
// 			addToStream(c, true);
// 		}
// 		APP_DBG("Video from %s opened\n", id.c_str());
// 	});

// 	client->audio = addAudio(
// 		pc, 8, 2, "AudioStream", "Stream",
// 		[id, wc = make_weak_ptr(client)]() {
// 			if (auto c = wc.lock()) {
// 				addToStream(c, false);
// 			}
// 			APP_DBG("Audio from %s opened\n", id.c_str());
// 		},
// 		id);


	auto dc = pc->createDataChannel("control");
	dc->onOpen([id, wcl = make_weak_ptr(client)]() {
		APP_DBG("[createDataChannel] open channel label");
		if (auto cl = wcl.lock()) {
			auto dc = cl->dataChannel.value();
			APP_DBG("open channel label: %s success\n", dc->label().c_str());
			// dc->send("Hello from " + mtce_getSerialInfo());
			APP_DBG("remove timeout connect\n");
			cl->removeTimeoutConnect();
			cl->mIsSignalingOk = true;
			if (++Client::totalClientsConnectSuccess > CLIENT_MAX) {
				APP_DBG("Client::totalClientsConnectSuccess > %d\n", CLIENT_MAX);
				FATAL("RTC", 0x01);
			}
			APP_DBG("total client: %d\n", Client::totalClientsConnectSuccess.load());

			auto pc = cl->peerConnection;
			Candidate iceCam, iceApp;
			if (pc->getSelectedCandidatePair(&iceCam, &iceApp)) {
				APP_DBG("local candidate selected: %s, transport type: %d\n", iceCam.candidate().c_str(), iceCam.transportType());
				APP_DBG("remote candidate selected: %s, transport type: %d\n", iceApp.candidate().c_str(), iceApp.transportType());
			}
			else {
				APP_DBG("get selected candidate pair failed\n");
			}
		}
	});

	dc->onClosed([id, wdc = make_weak_ptr(dc)]() {
		if (auto dc = wdc.lock()) {
			APP_DBG("DataChannel label: %s from: %s closed\n", dc->label().c_str(), id.c_str());
		}
	});

	dc->onMessage([id](auto data) {
		// data holds either string or rtc::binaryL
		if (holds_alternative<string>(data)) {
			json dataRev = {
				{"ClientId", id			   },
				  {"Data",	   get<string>(data)}
			 };
			task_post_dynamic_msg(GW_TASK_WEBRTC_ID, GW_WEBRTC_ON_MESSAGE_CONTROL_DATACHANNEL_REQ, (uint8_t *)dataRev.dump().c_str(), dataRev.dump().length() + 1);
		}
		else {
			APP_DBG("Binary message from %s received, size= %d\n", id.c_str(), get<rtc::binary>(data).size());
		}
	});

	dc->onBufferedAmountLow([id]() { APP_DBG("clientId %s send done\n", id.c_str()); });
	client->dataChannel = dc;

	// pc->setLocalDescription();
	return client;
};

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "app.h"
#include "app_data.h"
#include "app_dbg.h"
#include "parser_json.h" // This will also include <nlohmann/json.hpp> now
#include "utils.h"

using json = nlohmann::json; // Assuming you're using nlohmann::json, adjust if using a different JSON library
using namespace std;

bool mtce_jsonGetRtcServers(const json &json_in, rtcServersConfig_t *rtcSvCfg) {
    if (rtcSvCfg == nullptr) {
        APP_DBG("rtcSvCfg is null");
        return false;
    }

    try {
        rtcSvCfg->clear();
        json arrJs = json_in.at("Connections");
        
        for (const auto &server : arrJs) {
            if (server.contains("Type") && server["Type"].is_string()) {
                string type = server["Type"].get<string>();
                
                if (type == "stun") {
                    // Check if "StunUrl" is a string or an array and handle accordingly
                    if (server.contains("StunUrl")) {
                        if (server["StunUrl"].is_string()) {
                            rtcSvCfg->arrStunServerUrl.push_back(server["StunUrl"].get<string>());
                        } else if (server["StunUrl"].is_array() && !server["StunUrl"].empty()) {
                            rtcSvCfg->arrStunServerUrl.push_back(server["StunUrl"][0].get<string>());
                        }
                    }
                } else if (type == "turn") {
                    // Check if "TurnUrl" is a string or an array and handle accordingly
                    if (server.contains("TurnUrl")) {
                        if (server["TurnUrl"].is_string()) {
                            rtcSvCfg->arrTurnServerUrl.push_back(server["TurnUrl"].get<string>());
                        } else if (server["TurnUrl"].is_array() && !server["TurnUrl"].empty()) {
                            rtcSvCfg->arrTurnServerUrl.push_back(server["TurnUrl"][0].get<string>());
                        }
                    }
                } else if (type == "signaling") {
                    // Extract the signaling URL
                    if (server.contains("SignalingUrl") && server["SignalingUrl"].is_string()) {
                        rtcSvCfg->wSocketServerCfg = server["SignalingUrl"].get<string>();
                    } else {
                        APP_DBG("Warning: 'SignalingUrl' field is missing or not a string");
                    }
                }
            } else {
                APP_DBG("Warning: 'Type' field is missing or not a string");
            }
        }
        return true;
    }
    catch (const std::exception &e) {
        APP_DBG("json error: %s", e.what());
        return false;
    }
}

bool mtce_jsonGetEncode(json &json_in, mtce_encode_t *param) {
    try {
        // Main Format
        json mainFormat = json_in.at("MainFormat");
        json mainVideo = json_in.at("MainFormat").at("Video");
        json extraFormat = json_in.at("ExtraFormat");
        json extraVideo = json_in.at("ExtraFormat").at("Video");

        param->mainFmt.bAudioEnable = json_in["MainFormat"]["AudioEnable"].get<bool>();
        param->mainFmt.bVideoEnable = json_in["MainFormat"]["VideoEnable"].get<bool>();

        APP_DBG("MainFormat - AudioEnable: %d\n", param->mainFmt.bAudioEnable);
        APP_DBG("MainFormat - VideoEnable: %d\n", param->mainFmt.bVideoEnable);

        string str = json_in["MainFormat"]["Video"]["Compression"].get<string>();
        param->mainFmt.format.compression = (str == "H.264") ? MTCE_CAPTURE_COMP_H264 :
                                           (str == "H.265") ? MTCE_CAPTURE_COMP_H265 :
                                                              MTCE_CAPTURE_COMP_NONE;

        APP_DBG("MainFormat - Compression: %s\n", str.c_str());

        str = json_in["MainFormat"]["Video"]["Resolution"].get<string>();
        param->mainFmt.format.resolution = (str == "2K") ? MTCE_CAPTURE_SIZE_2K :
                                          (str == "1080P") ? MTCE_CAPTURE_SIZE_1080P :
                                          (str == "720P") ? MTCE_CAPTURE_SIZE_720P :
                                          (str == "480P") ? MTCE_CAPTURE_SIZE_VGA :
                                                            MTCE_CAPTURE_SIZE_NONE;

        APP_DBG("MainFormat - Resolution: %s\n", str.c_str());

        param->mainFmt.format.bitRate = json_in["MainFormat"]["Video"]["BitRate"].get<int>();
        param->mainFmt.format.virtualGOP = json_in["MainFormat"]["Video"]["VirtualGOP"].get<int>();
        param->mainFmt.format.FPS = json_in["MainFormat"]["Video"]["FPS"].get<int>();
        param->mainFmt.format.quality = json_in["MainFormat"]["Video"]["Quality"].get<int>();
        param->mainFmt.format.GOP = json_in["MainFormat"]["Video"]["GOP"].get<int>();

        APP_DBG("MainFormat - BitRate: %d\n", param->mainFmt.format.bitRate);
        APP_DBG("MainFormat - VirtualGOP: %d\n", param->mainFmt.format.virtualGOP);
        APP_DBG("MainFormat - FPS: %d\n", param->mainFmt.format.FPS);
        APP_DBG("MainFormat - Quality: %d\n", param->mainFmt.format.quality);
        APP_DBG("MainFormat - GOP: %d\n", param->mainFmt.format.GOP);

        // Extra Format
        param->extraFmt.bAudioEnable = json_in["ExtraFormat"]["AudioEnable"].get<bool>();
        param->extraFmt.bVideoEnable = json_in["ExtraFormat"]["VideoEnable"].get<bool>();

        APP_DBG("ExtraFormat - AudioEnable: %d\n", param->extraFmt.bAudioEnable);
        APP_DBG("ExtraFormat - VideoEnable: %d\n", param->extraFmt.bVideoEnable);

        str = json_in["ExtraFormat"]["Video"]["Compression"].get<string>();
        param->extraFmt.format.compression = (str == "H.264") ? MTCE_CAPTURE_COMP_H264 :
                                           (str == "H.265") ? MTCE_CAPTURE_COMP_H265 :
                                                              MTCE_CAPTURE_COMP_NONE;

        APP_DBG("ExtraFormat - Compression: %s\n", str.c_str());

        str = json_in["ExtraFormat"]["Video"]["Resolution"].get<string>();
        param->extraFmt.format.resolution = (str == "2K") ? MTCE_CAPTURE_SIZE_2K :
                                          (str == "1080P") ? MTCE_CAPTURE_SIZE_1080P :
                                          (str == "720P") ? MTCE_CAPTURE_SIZE_720P :
                                          (str == "480P") ? MTCE_CAPTURE_SIZE_VGA :
                                                            MTCE_CAPTURE_SIZE_NONE;

        APP_DBG("ExtraFormat - Resolution: %s\n", str.c_str());

        param->extraFmt.format.bitRate = json_in["ExtraFormat"]["Video"]["BitRate"].get<int>();
        param->extraFmt.format.virtualGOP = json_in["ExtraFormat"]["Video"]["VirtualGOP"].get<int>();
        param->extraFmt.format.FPS = json_in["ExtraFormat"]["Video"]["FPS"].get<int>();
        param->extraFmt.format.quality = json_in["ExtraFormat"]["Video"]["Quality"].get<int>();
        param->extraFmt.format.GOP = json_in["ExtraFormat"]["Video"]["GOP"].get<int>();

        APP_DBG("ExtraFormat - BitRate: %d\n", param->extraFmt.format.bitRate);
        APP_DBG("ExtraFormat - VirtualGOP: %d\n", param->extraFmt.format.virtualGOP);
        APP_DBG("ExtraFormat - FPS: %d\n", param->extraFmt.format.FPS);
        APP_DBG("ExtraFormat - Quality: %d\n", param->extraFmt.format.quality);
        APP_DBG("ExtraFormat - GOP: %d\n", param->extraFmt.format.GOP);

        return true;
    }
    catch (const std::exception &e) {
        APP_DBG("json error: %s", e.what());
        return false;
    }
}

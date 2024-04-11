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
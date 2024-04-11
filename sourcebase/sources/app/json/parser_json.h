#ifndef PARSER_JSON_H
#define PARSER_JSON_H

#include "app_data.h" // Ensure this header contains rtcServersConfig_t
#include <json.hpp> // Include nlohmann/json.hpp here so that the json type is known
#include "json.hpp" // Correct path relative to the project's root


extern bool mtce_jsonGetRtcServers(const json &json_in, rtcServersConfig_t *rtcSvCfg);

#endif // PARSER_JSON_H

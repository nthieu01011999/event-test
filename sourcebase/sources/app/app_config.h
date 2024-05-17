#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <stdint.h>
#include <string> // Include for std::string
#include "json.hpp" // Include for nlohmann::json

#include "ak.h"
#include "app.h"
#include "app_data.h"
#include "ak_dbg.h"
#include "sys_dbg.h"

using namespace std;
using json = nlohmann::json; // Alias nlohmann::json to json

#define APP_CONFIG_SUCCESS             (0)
#define APP_CONFIG_ERROR_FILE_OPEN     (-1)
#define APP_CONFIG_ERROR_DATA_MALLOC   (-2)
#define APP_CONFIG_ERROR_DATA_INVALID  (-3)
#define APP_CONFIG_ERROR_DATA_DIFF     (-4)
#define APP_CONFIG_ERROR_TIMEOUT       (-5)
#define APP_CONFIG_ERROR_BUSY          (-6)
#define APP_CONFIG_ERROR_ANOTHER       (-7)

extern int configGetRtcServers(rtcServersConfig_t *rtcServerCfg);
int mtce_readConfigUsrDfaulFileToJs(json &cfgJs, string &file);
extern int mtce_configGetSerial(char *serial, size_t size);
extern int mtce_configGetEncode(mtce_encode_t *encodeCfg);
#endif    // __APP_CONFIG_H__

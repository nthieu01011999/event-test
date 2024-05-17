#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#include <string.h>

#include <iostream>
#include <cstring>

using namespace std;

#include "utils.h"

#include "app.h"
#include "app_data.h"
#include "app_config.h"

#include "app_dbg.h"
// #include "app_config.h"
#include "parser_json.h"
#include "vvtk_system.h"
// #include "ota.h"
/******************************************************************************
 * APP CONFIGURE CLASS (BASE)
 ******************************************************************************/

#include <vector>

int configGetRtcServers(rtcServersConfig_t *rtcServerCfg) {
	json cfgJs;

	string fileName = "RtcServersConfig";
	if (mtce_readConfigUsrDfaulFileToJs(cfgJs, fileName)) {
		return APP_CONFIG_ERROR_FILE_OPEN;
	}

	if (mtce_jsonGetRtcServers(cfgJs, rtcServerCfg)) {
		APP_DBG("rtc server config: %s\n", cfgJs.dump().data());
		return APP_CONFIG_SUCCESS;
	}
	else {
		APP_DBG("Can not convert: %s\n", fileName.data());
		return APP_CONFIG_ERROR_DATA_INVALID;
	}
}

int mtce_readConfigUsrDfaulFileToJs(json &cfgJs, string &file) {
	string filePath = "/usr/conf" + string("/") + file;
	if (!read_json_file(cfgJs, filePath)) {
		APP_DBG("Can not read: %s\n", filePath.data());
		filePath = "/app/default" + string("/") + file;
		cfgJs.clear();
		if (!read_json_file(cfgJs, filePath)) {
			APP_DBG("Can not read: %s\n", filePath.data());
			return APP_CONFIG_ERROR_FILE_OPEN;
		}
	}
	return APP_CONFIG_SUCCESS;
}

int mtce_configGetSerial(char *serial, size_t size) {
	char sn[128]	   = {0};
	char sn_lower[128] = {0};
	int i;
	int ret = vvtk_system_get_sn(sn, sizeof(sn));
	if (ret != VVTK_RET_SUCCESS) {
		printf("vvtk_system_get_sn() fail with retcode: %d\n", ret);
		safe_strcpy(serial, MTCE_SERIAL_NUM_DEFAULT, size);
	}
	else {
		if (strlen(sn) == 0 || sn == NULL)
			safe_strcpy(serial, MTCE_SERIAL_NUM_DEFAULT, size);
		else {
			int len = strlen(sn);
			for (i = 0; i < len; i++) {
				sn_lower[i] = tolower(sn[i]);
			}

			string strlist = sn_lower;

			std::vector<std::string> v = string_split(strlist, "-");

			if (v.size() >= 4) {
				safe_strcpy(sn_lower, v[3].c_str(), sizeof(sn_lower));
				len = v[3].length();
				if (len > 4 && sn_lower[len - 3] == ':') {
					sn_lower[len - 3] = '\0';
				}
			}

			safe_strcpy(serial, sn_lower, size);
		}
	}

	APP_DBG("serialNo: %s\n", serial);
	return -1;
}

int mtce_configGetEncode(mtce_encode_t *encodeCfg) {
	json cfgJs;

	string fileName = MTCE_ENCODE_FILE;
	if (mtce_readConfigUsrDfaulFileToJs(cfgJs, fileName)) {
		return APP_CONFIG_ERROR_FILE_OPEN;
	}

	if (mtce_jsonGetEncode(cfgJs, encodeCfg)) {
		APP_DBG("Encode config: %s\n", cfgJs.dump().data());
		return APP_CONFIG_SUCCESS;
	}
	else {
		APP_DBG("Can not convert: %s\n", fileName.data());
		return APP_CONFIG_ERROR_DATA_INVALID;
	}
}
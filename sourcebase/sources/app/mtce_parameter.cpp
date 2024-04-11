#include "app_config.h"
#include "mtce_parameter.h"

mtce_deviceInfo_t mtce_deviceInfo;

int mtce_contextInit() {
	memset(&mtce_deviceInfo, 0, sizeof(mtce_deviceInfo));
	mtce_configGetSerial(mtce_deviceInfo.serial, sizeof(mtce_deviceInfo.serial));
	return 0;
}

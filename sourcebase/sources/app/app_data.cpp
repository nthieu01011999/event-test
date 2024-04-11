#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ak.h"

#include "rtc/rtc.hpp"
#include "task_list.h"
#include "app_config.h"
#include "app_data.h"
  
std::string mtce_getSerialInfo() {
	return std::string(mtce_deviceInfo.serial, strlen(mtce_deviceInfo.serial));
}
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

ClientsGroup_t clients;
static pthread_mutex_t mtxListClients = PTHREAD_MUTEX_INITIALIZER;

void lockMutexListClients() {
	pthread_mutex_lock(&mtxListClients);
}

void unlockMutexListClients() {
	pthread_mutex_unlock(&mtxListClients);
}

std::string mtce_getSerialInfo() {
	return std::string(mtce_deviceInfo.serial, strlen(mtce_deviceInfo.serial));
}

void sendMsgControlDataChannel(const string &id, const string &msg) {
	if (msg.empty())
		return;

	lockMutexListClients();
	if (auto jt = clients.find(id); jt != clients.end()) {
		auto dc = jt->second->dataChannel.value();
		APP_PRINT("Send message to %s\n", id.c_str());
		try {
			if (dc->isOpen()) {
				dc->send(msg);
			}
		}
		catch (const exception &error) {
			APP_DBG("%s\n", error.what());
		}
	}
	else {
		APP_PRINT("%s not found\n", id.c_str());
	}
	unlockMutexListClients();
}

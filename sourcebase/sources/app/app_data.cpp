#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/resource.h>
#include <iostream>

#include "ak.h"

#include "rtc/rtc.hpp"
#include "task_list.h"
#include "app_config.h"
#include "app_data.h"
#include "h26xsource.hpp"



extern VideoCtrl videoCtrl;
static pthread_mutex_t mtxStreamVideo = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mtxListClients = PTHREAD_MUTEX_INITIALIZER;


ClientsGroup_t clients;
 
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

void printMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cout << "Memory usage: " << usage.ru_maxrss << " KB" << std::endl;
}


void onSampleVideoCapture(int channel, uint8_t *bytes, uint32_t nbBytes) {
    
    // Existing video capture processing code
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::thread::id this_id = std::this_thread::get_id();

    if (videoCtrl.videoForceStopStream) {
        return;
    }

	bool isFullHD = false;
	if (channel == MTCE_MAIN_STREAM) {
		isFullHD = true;
	}
	
	Stream::pubLicStreamPOSIXMutexLOCK();
	auto nalUnits = H26XSource::ExtractSeqNALUS(bytes, nbBytes);
	if (nalUnits.size()) {
		auto &refNalUnits = Stream::nalUnitsMain;
		if (isFullHD) {
			refNalUnits = Stream::nalUnitsMain;
		} 
		else {
			refNalUnits = Stream::nalUnitsSub;
		}
		refNalUnits.clear();
		refNalUnits.shrink_to_fit();
		refNalUnits.assign(nalUnits.begin(), nalUnits.end());
	}
	Stream::pubLicStreamPOSIXMutexUNLOCK();


	pthread_mutex_lock(&mtxStreamVideo);
	Stream::MediaLiveVideo(isFullHD, bytes, nbBytes);
	pthread_mutex_unlock(&mtxStreamVideo);
}
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include "mtce_video.hpp"
#include "stream.hpp"
#include "h26xsource.hpp"



#define VIDEO_DEBUG 1

extern VideoCtrl videoCtrl;
static pthread_mutex_t mtxStreamVideo = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mtxListClients = PTHREAD_MUTEX_INITIALIZER;

VideoCtrl::VideoCtrl() {
	APP_DBG("[video] Init class VideoCtrl\n");
	videoForceStopStream = false;
}

VideoCtrl::~VideoCtrl() {
	APP_DBG("[video] Release class VideoCtrl\n");
	stopStreamAllChannels();
}

void VideoCtrl::stopStreamAllChannels() {
	int i;
	for (i = 0; i < MTCE_MAX_STREAM_NUM; i++) {
		// mVideoChn[i].value()->stopCapture(i);
	}
}

bool VideoCtrl::initializeCamera(int channel) {
	vvtk_video_config_t config;
	config.codec = VVTK_VIDEO_CODEC_H264;
	config.width = 1920;
	config.height = 1080;
	config.frame_rate = 30;
	config.bitrate_max = 5000;
	config.encoding_mode = VVTK_VIDEO_ENCODING_MODE_CBR;

	VVTK_RET result = vvtk_set_video_config(channel, &config);
	if(result == VVTK_RET_SUCCESS) {
		APP_DBG("Camera initialized successfully on channel %d\n", channel);
	}
	else {
		APP_DBG("Failed to initialize camera on channel %d\n", channel);
	}
}

void VideoCtrl::startCapture(int channel) {
	if(!initializeCamera(channel)) {
		APP_DBG("Camera on channel %d is not initialized.\n", channel);
		return;
	}

	VVTK_VIDEO_CALLBACK cb;
	switch (channel)
	{
	case MTCE_MAIN_STREAM:
		cb = onStartMainChannel;
		break;
	case MTCE_SUB_STREAM:
		cb = onStartSubChannel;
		break;
	default:
		APP_DBG("Invalid channel %d\n", channel);
		return;
		break;
	}
	
	if (vvtk_set_video_callback(channel, cb, nullptr) == VVTK_RET_SUCCESS) {
		APP_DBG("Capture started on channel %d\n", channel);
		streamIsRunning = true;
	} else {
		throw std::runtime_error("[VIDEO] Failed to register video callback.\n");
	}

}

void VideoCtrl::stopCapture(int channel) {


	VVTK_VIDEO_CALLBACK cb;
	switch (channel)
	{
	case MTCE_MAIN_STREAM:
		cb = onStartMainChannel;
		break;
	case MTCE_SUB_STREAM:
		cb = onStartSubChannel;
		break;
	default:
		APP_DBG("Invalid channel %d\n", channel);
		return;
		break;
	}
	
	if (vvtk_set_video_callback(channel, cb, nullptr) == VVTK_RET_SUCCESS) {
		APP_DBG("Capture started on channel %d\n", channel);
		streamIsRunning = false;
	} else {
		APP_DBG("[VIDEO] stop stream on channel %d error\n", channel);
	}

}

extern "C" VVTK_RET_CALLBACK onStartMainChannel(const vvtk_video_frame_t *videoFrame, const void *arg) {
	(void)videoFrame;
	(void)arg;
	if (videoFrame->size > (int)sizeof(rtc::NalUnitHeader)) {
		videoCtrl.captureFrame(MTCE_MAIN_STREAM, videoFrame->data, videoFrame->size);
	}

	return VVTK_RET_CALLBACK_CONTINUE;
}

extern "C" VVTK_RET_CALLBACK onStartSubChannel(const vvtk_video_frame_t *videoFrame, const void *arg) {
	(void)videoFrame;
	(void)arg;
	if (videoFrame->size > (int)sizeof(rtc::NalUnitHeader)) {
		videoCtrl.captureFrame(MTCE_SUB_STREAM, videoFrame->data, videoFrame->size);
	}

	return VVTK_RET_CALLBACK_CONTINUE;
}

void VideoCtrl::captureFrame(int channel, uint8_t *bytes, uint32_t nbBytes) {
//     if (videoCtrl.videoForceStopStream) {
//         return;
//     }

//     bool isFullHD = (channel == MTCE_MAIN_STREAM);

//     Stream::pubLicStreamPOSIXMutexLOCK();
    
//     auto nalUnits = H26XSource::ExtractSeqNALUS(bytes, nbBytes);
//     if (!nalUnits.empty()) {
//         auto &refNalUnits = Stream::nalUnitsMain; // Default to main channel
        
//         switch (channel) {
//             case MTCE_MAIN_STREAM:
//                 refNalUnits = Stream::nalUnitsMain;
//                 break;
//             case MTCE_SUB_STREAM:
//                 refNalUnits = Stream::nalUnitsSub;
//                 break;
//             default:
//                 break;
//         }
        
//         refNalUnits.clear();
//         refNalUnits.shrink_to_fit();
//         refNalUnits.assign(nalUnits.begin(), nalUnits.end());
//     }

//     Stream::pubLicStreamPOSIXMutexUNLOCK();

//     pthread_mutex_lock(&mtxStreamVideo);
//     Stream::MediaLiveVideo(isFullHD, bytes, nbBytes);
//     pthread_mutex_unlock(&mtxStreamVideo);
}

bool VideoCtrl::getStreamIsRunningChannel(int channel) {
    if (channel < MTCE_MAX_STREAM_NUM && channel >= 0) {
        // if (mVideoChn[channel]) {
        //     return mVideoChn[channel].value()->streamIsRunning;
        // }
    }
    return false;
}
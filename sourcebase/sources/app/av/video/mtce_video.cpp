#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>

#include "app_data.h"
#include "app_config.h"
#include "mtce_video.hpp"
#include "parser_json.h"
#include "task_list.h"

 
// VideoChannel Class Definitions
static mtce_sizePicture_t size_of_picture[] = {
	[MTCE_CAPTURE_SIZE_VGA]	  = {640,  360 },
	[MTCE_CAPTURE_SIZE_720P]  = {1280, 720 },
	[MTCE_CAPTURE_SIZE_1080P] = {1920, 1080},
	[MTCE_CAPTURE_SIZE_2K]	  = {2304, 1296},
	[MTCE_CAPTURE_SIZE_NONE]  = {0,	0	 }
};
// Constructor
VideoChannel::VideoChannel() : mWidth(0), mHeight(0), streamIsRunning(false) {
    std::cout << "Initializing VideoChannel with default settings." << std::endl;
}

// Destructor
VideoChannel::~VideoChannel() {
    std::cout << "Destroying VideoChannel instance." << std::endl;
}

int VideoChannel::applyConf(int channel, const mtce_mediaFormat_t *mediaFormat) {
    int err;
    vvtk_video_config_t videoConf;

    // First, get the current video configuration
    err = vvtk_get_video_config(channel, &videoConf);
    if (err != 0) {
        APP_DBG("[video] get encode config channel %d error: %d\n", channel, err);
        return -1;
    }

    // Set codec based on the input media format
    videoConf.codec = (mediaFormat->format.compression == MTCE_CAPTURE_COMP_H264) ? VVTK_VIDEO_CODEC_H264 : VVTK_VIDEO_CODEC_H265;

    // Set resolution based on predefined settings
    mtce_sizePicture_t size_pic = size_of_picture[mediaFormat->format.resolution];
    videoConf.width = size_pic.width;
    videoConf.height = size_pic.height;

    // Set a default frame rate for testing
    videoConf.frame_rate = 30; // Default to 30 FPS for basic testing

    // Apply the updated video configuration
    err = vvtk_set_video_config(channel, &videoConf);
    if (err != 0) {
        APP_DBG("[video] encode set config channel %d error: %d\n", channel, err);
        return -1;
    }

    APP_DBG("[video] config encode channel %d success\n", channel);
    APP_DBG("[video] set resolution [%dx%d], codec %s\n", videoConf.width, videoConf.height, (videoConf.codec == VVTK_VIDEO_CODEC_H264 ? "H264" : "H265"));

    return 0;
}


// Set configuration for the video channel based on resolution
void VideoChannel::setConfChannel(const mtce_mediaFormat_t *conf) {
	mtce_sizePicture_t size_pic = size_of_picture[conf->format.resolution];
	mWidth						= size_pic.width;
	mHeight						= size_pic.height;
}

// Start video streaming on a specific channel
int VideoChannel::startStream(int channel) {
    if (!streamIsRunning) {
        streamIsRunning = true;
        std::cout << "Starting video stream on channel " << channel << " at resolution " << mWidth << "x" << mHeight << "." << std::endl;
        // Here you would typically invoke lower-level API calls to start the hardware or software stream
        // For example:
        // hardwareAPI.startCamera(mWidth, mHeight);
    } else {
        std::cout << "Stream already running on channel " << channel << "." << std::endl;
        return -1; // Stream already running
    }
    return 0; // Success
}

// Stop video streaming on a specific channel
void VideoChannel::stopStream(int channel) {
    if (streamIsRunning) {
        streamIsRunning = false;
        std::cout << "Stopping video stream on channel " << channel << "." << std::endl;
        // Similar to startStream, you would have a hardware or software call to stop the stream
        // hardwareAPI.stopCamera();
    } else {
        std::cout << "Stream not running on channel " << channel << ", nothing to stop." << std::endl;
    }
}

// VideoCtrl Class Definitions

// Constructor
VideoCtrl::VideoCtrl() : mInitialized(false) {
    std::cout << "VideoCtrl initialized. Ready to configure channels." << std::endl;
}

// Destructor
VideoCtrl::~VideoCtrl() {
    std::cout << "Cleaning up VideoCtrl resources." << std::endl;
}


int VideoCtrl::setVideoEncodeChannels(mtce_encode_t *encodeConf) {
	int err, i;
	mtce_mediaFormat_t *conf;
	for (i = 0; i < MTCE_MAX_STREAM_NUM; i++) {
		conf = (mtce_mediaFormat_t *)encodeConf + i;
		mVideoChn[i].setConfChannel(conf);

		/*apply config*/
		err = mVideoChn[i].applyConf(i, conf);
		if (err != 0) {
			APP_DBG("[video] encode set config at channel %d err\n", i);
			return -1;
		}

		APP_DBG("+++ %s channel +++\n", (i == MTCE_MAIN_STREAM) ? "MAIN" : "SUB");
		APP_DBG("    - Encode          :	%s\n", (conf->format.compression == MTCE_CAPTURE_COMP_H264) ? "H264" : "H265");
		APP_DBG("    - Bitrate control :	%s\n", (conf->format.bitRateControl == MTCE_CAPTURE_BRC_CBR) ? "CBR" : "VBR");
		APP_DBG("    - Resolution      :	%d\n", conf->format.resolution);
		APP_DBG("    - FPS             :	%d\n", conf->format.FPS);
		APP_DBG("    - GOP             :	%d\n", conf->format.GOP);
		APP_DBG("    - Bitreate        :	%d\n", conf->format.bitRate);
		APP_DBG("    - Quality         :	%d\n", conf->format.quality);
		APP_DBG("\n");
	}
	return 0;
}

// Start streams on all configured channels
void VideoCtrl::startStreamAllChannels() {
    std::cout << "Attempting to start all video streams." << std::endl;
    for (int i = 0; i < 4; ++i) { // Assuming 4 channels for simplicity
        if (!mVideoChn[i].streamIsRunning) {
            mVideoChn[i].startStream(i);
        }
    }
    setInitialized(true);
}

// Stop streams on all channels
void VideoCtrl::stopStreamAllChannels() {
    std::cout << "Attempting to stop all video streams." << std::endl;
    for (int i = 0; i < 4; ++i) {
        if (mVideoChn[i].streamIsRunning) {
            mVideoChn[i].stopStream(i);
        }
    }
    setInitialized(false);
}

// Return the initialization status
bool VideoCtrl::initialized() const {
    return mInitialized.load();
}

// Set the initialization status
void VideoCtrl::setInitialized(bool newInitialized) {
    mInitialized = newInitialized;
    std::cout << "Initialization status set to " << (newInitialized ? "true" : "false") << std::endl;
}

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

    // Simplified assumption: Only handling H264 and H265 for testing
    videoConf.codec = (mediaFormat->format.compression == MTCE_CAPTURE_COMP_H264) ? VVTK_VIDEO_CODEC_H264 : VVTK_VIDEO_CODEC_H265;

    // Set resolution directly from a predefined array based on input format
    mtce_sizePicture_t size_pic = size_of_picture[mediaFormat->format.resolution];
    videoConf.width = size_pic.width;
    videoConf.height = size_pic.height;

    // Set a default frame rate for testing
    videoConf.frame_rate = 30; // Default to 30 FPS for basic testing

    // Set the video configuration
    err = vvtk_set_video_config(channel, &videoConf);
    if (err != 0) {
        APP_DBG("[video] encode set config channel %d error: %d\n", channel, err);
        return -1;
    }

    APP_DBG("[video] config encode channel %d success\n", channel);
    APP_DBG("[video] set resolution [%dx%d]\n", videoConf.width, videoConf.height);

    return 0;
}

// Set configuration for the video channel based on resolution
void VideoChannel::setConfChannel(const mtce_sizePicture_t &size) {
    mWidth = size.width;
    mHeight = size.height;
    std::cout << "Configuration set: Width = " << mWidth << ", Height = " << mHeight << std::endl;
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

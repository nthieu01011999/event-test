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
    if (mediaFormat == nullptr) {
        APP_DBG("[applyConf] Error: mediaFormat pointer is null for channel %d\n", channel);
        return -1;
    }

    vvtk_video_config_t videoConf;

    APP_DBG("[applyConf] Attempting to retrieve current video configuration for channel %d\n", channel);

    // videoConf.codec = VVTK_VIDEO_CODEC_H264;
    // videoConf.width = 1280;
    // videoConf.height = 720;
    // videoConf.encoding_mode = VVTK_VIDEO_ENCODING_MODE_VBR;
    // videoConf.bitrate_max = 1024 * 1024;
    // videoConf.bitrate_min = videoConf.bitrate_max * 2/3;
    // videoConf.frame_rate = 13; //FPS

    int gopCal = 13 * 13;  // "GOP" times "FPS"
    videoConf.gop = gopCal > MTCE_MAX_GOP ? MTCE_MAX_GOP : gopCal;   

     videoConf.vbr_quality = static_cast<VVTK_VBR_QUALITY>(20);   

    // Apply the configuration
    err = vvtk_set_video_config(channel, &videoConf);
    if (err != 0) {
        APP_DBG("[video] encode set config channel %d err: %d\n", channel, err);
        return -1;
    }

    // Print all configuration details
    APP_DBG("Video Configuration Details:\n");
    APP_DBG("Codec: %d\n", videoConf.codec);
    APP_DBG("Width: %d pixels\n", videoConf.width);
    APP_DBG("Height: %d pixels\n", videoConf.height);
    APP_DBG("Frame Rate: %d FPS\n", videoConf.frame_rate);
    APP_DBG("Encoding Mode: %d\n", videoConf.encoding_mode);
    APP_DBG("GOP Size: %d\n", videoConf.gop);
    APP_DBG("Min Bitrate: %d Kbps\n", videoConf.bitrate_min);
    APP_DBG("Max Bitrate: %d Kbps\n", videoConf.bitrate_max);
    APP_DBG("VBR Quality Level: %d\n", videoConf.vbr_quality);

    return 0;
}
VVTK_VBR_QUALITY VideoChannel::qualityMaps(int mode) {
	VVTK_VBR_QUALITY quality = VVTK_VBR_QUALITY_NORMAL;

	switch (mode) {
	case MTCE_CAPTURE_QUALITY_LOWER: {
		quality = VVTK_VBR_QUALITY_LOWER;
	} break;
	case MTCE_CAPTURE_QUALITY_LOW: {
		quality = VVTK_VBR_QUALITY_LOW;
	} break;
	case MTCE_CAPTURE_QUALITY_NORMAL: {
		quality = VVTK_VBR_QUALITY_NORMAL;
	} break;
	case MTCE_CAPTURE_QUALITY_GOOD: {
		quality = VVTK_VBR_QUALITY_GOOD;
	} break;
	case MTCE_CAPTURE_QUALITY_VERY_GOOD: {
		quality = VVTK_VBR_QUALITY_VERY_GOOD;
	} break;
	case MTCE_CAPTURE_QUALITY_BETTER: {
		quality = VVTK_VBR_QUALITY_BETTER;
	} break;
	case MTCE_CAPTURE_QUALITY_EXCELLENT: {
		quality = VVTK_VBR_QUALITY_EXCELLENT;
	} break;
	case MTCE_CAPTURE_QUALITY_HIGH: {
		quality = VVTK_VBR_QUALITY_VERY_HIGH;
	} break;

	default:
		quality = VVTK_VBR_QUALITY_NORMAL;
		break;
	}

	return quality;
}

// int VideoChannel::applyConf(int channel, const mtce_mediaFormat_t *mediaFormat) {
// 	int err, gopCal;
// 	vvtk_video_config_t videoConf, tmpConf;
// 	VVTK_VIDEO_ENCODING_MODE modeSet;
// 	VVTK_VIDEO_CODEC codecSet;

// 	if (mediaFormat->format.compression == MTCE_CAPTURE_COMP_H264)
// 		codecSet = VVTK_VIDEO_CODEC_H264;
// 	else
// 		codecSet = VVTK_VIDEO_CODEC_H265;

// 	if (mediaFormat->format.bitRateControl == MTCE_CAPTURE_BRC_CBR)
// 		modeSet = VVTK_VIDEO_ENCODING_MODE_CBR;
// 	else if (mediaFormat->format.bitRateControl == MTCE_CAPTURE_BRC_VBR)
// 		modeSet = VVTK_VIDEO_ENCODING_MODE_VBR;
// 	else
// 		modeSet = VVTK_VIDEO_ENCODING_MODE_CVBR;
// APP_DBG("[BEFORE] get encode config channel %d err: %d\n", channel, err);
// 	// err = vvtk_get_video_config(channel, &videoConf);
// 	// if (err != 0) {
// 	// 	APP_DBG("[video] get encode config channel %d err: %d\n", channel, err);
// 	// 	return -1;
// 	// }

// 	videoConf.encoding_mode = modeSet;
// 	if (videoConf.encoding_mode == VVTK_VIDEO_ENCODING_MODE_VBR) {
// 		videoConf.vbr_quality = qualityMaps(mediaFormat->format.quality);
// 	}
// 	else if (videoConf.encoding_mode == VVTK_VIDEO_ENCODING_MODE_CBR) {
// 		videoConf.bitrate_max = mediaFormat->format.bitRate * 1024;
// 	}
// 	else {
// 		videoConf.bitrate_max = mediaFormat->format.bitRate * 1024;
// 		videoConf.bitrate_min = videoConf.bitrate_max * 2 / 3;
// 	}
// 	videoConf.codec		 = codecSet;
// 	videoConf.frame_rate = mediaFormat->format.FPS;
// 	gopCal = mediaFormat->format.GOP * mediaFormat->format.FPS;
// 	videoConf.gop		 = gopCal > MTCE_MAX_GOP ? MTCE_MAX_GOP : gopCal;
// 	videoConf.width		 = mWidth;
// 	videoConf.height	 = mHeight;
// 	err					 = vvtk_set_video_config(channel, &videoConf);

//     APP_DBG("[AFTER] get encode config channel %d err: %d\n", channel, err);

// // 	if (err != 0) {
// // 		APP_DBG("[video] encode set config channel %d err: %d\n", channel, err);
// // 		return -1;
// // 	}

// // 	err = vvtk_get_video_config(channel, &tmpConf);
// // 	if (err != 0) {
// // 		APP_DBG("[video] encode get config channel %d err: %d\n", channel, err);
// // 		return -1;
// // 	}

// // 	if (memcmp(&videoConf, &tmpConf, sizeof(tmpConf)) != 0) {
// // 		APP_DBG("[video] diff param set and get encode config channel %d\n", channel);
// // 		APP_DBG(" +++ Difference set & get +++\n");
// // #ifndef RELEASE
// // 		diffSetAndGetConf(&videoConf, &tmpConf);
// // #endif
// // 		APP_DBG("\n");
// // 	}

// // 	APP_DBG("[video] config encode channel %d success\n", channel);
// // 	APP_DBG("[IMPORTANTTTTTTTTTTTTTTTTTT] size resolution [%dx%d]\n", videoConf.width, videoConf.height);

// 	return 0;
// }

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






void VideoChannel::diffSetAndGetConf(vvtk_video_config_t *videoConf, vvtk_video_config_t *tmpConf) {
	if (videoConf->codec != tmpConf->codec) {
		APP_DBG("- Diff parma encode Type: set %s & get %s", ((videoConf->codec == VVTK_VIDEO_CODEC_H264) ? "h264" : "h265"),
				((tmpConf->codec == VVTK_VIDEO_CODEC_H264) ? "h264" : "h265"));
	}
	else if (videoConf->frame_rate != tmpConf->frame_rate) {
		APP_DBG("- Diff param FPS: set %d & get %d", videoConf->frame_rate, tmpConf->frame_rate);
	}
	else if (videoConf->gop != tmpConf->gop) {
		APP_DBG("- Diff param GOP: set %d & get %d", videoConf->gop, tmpConf->gop);
	}
	else if (videoConf->encoding_mode != tmpConf->encoding_mode) {
		APP_DBG("- Diff param bitrate Mode: set %s & get %s",
				((videoConf->encoding_mode == VVTK_VIDEO_ENCODING_MODE_VBR)	  ? "VBR"
				 : (videoConf->encoding_mode == VVTK_VIDEO_ENCODING_MODE_CBR) ? "CBR"
																			  : "CVBR"),
				((tmpConf->encoding_mode == VVTK_VIDEO_ENCODING_MODE_VBR)	? "VBR"
				 : (tmpConf->encoding_mode == VVTK_VIDEO_ENCODING_MODE_CBR) ? "CBR"
																			: "CVBR"));
	}
	else if (videoConf->vbr_quality != tmpConf->vbr_quality) {
		if (videoConf->encoding_mode == VVTK_VIDEO_ENCODING_MODE_VBR)
			APP_DBG("- Diff param Quality: set %d & get %d", videoConf->vbr_quality, tmpConf->vbr_quality);
	}
	else if (videoConf->bitrate_max != tmpConf->bitrate_max || videoConf->bitrate_min != tmpConf->bitrate_min) {
		if (videoConf->encoding_mode == VVTK_VIDEO_ENCODING_MODE_CVBR) {
			APP_DBG("- Diff param bitrate max: set %d & get %d", videoConf->bitrate_max, tmpConf->bitrate_max);
			APP_DBG("- Diff param bitrate min: set %d & get %d", videoConf->bitrate_min, tmpConf->bitrate_min);
		}
		else if (videoConf->encoding_mode == VVTK_VIDEO_ENCODING_MODE_CBR) {
			APP_DBG("- Diff param bitrate max: set %d & get %d", videoConf->bitrate_max, tmpConf->bitrate_max);
		}
	}
	else if (videoConf->width != tmpConf->width || videoConf->height != tmpConf->height) {
		APP_DBG("- Diff parma set resolution: set [%dx%d] & get [%dx%d]", videoConf->width, videoConf->height, tmpConf->width, tmpConf->height);
	}
	else {
		APP_DBG("[video] not diff set and get config");
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

int VideoCtrl::loadConfigFromFileChannels(mtce_encode_t *encodeConf) {
	int err;

	err = mtce_configGetEncode(encodeConf);
	if (err != 0) {
		APP_DBG("[video] get config from to file err\n");
		return err;
	}

	// err = verifyConfig(encodeConf);
	// if (err != 0) {
	// 	APP_DBG("[video] Verify config encode err\n");
	// 	return err;
	// }

	return APP_CONFIG_SUCCESS;
}

int VideoCtrl::setVideoEncodeChannels(mtce_encode_t *encodeConf) {

	int err, i;
	mtce_mediaFormat_t *conf;
	for (i = 0; i < MTCE_MAX_STREAM_NUM; i++) {
		conf = (mtce_mediaFormat_t *)encodeConf + i;
		mVideoChn[i].setConfChannel(conf);
    
    APP_DBG("[VideoCtrl] setVideoEncodeChannels.\n");
	// 	/*apply config*/
		err = mVideoChn[i].applyConf(i, conf);
		// if (err != 0) {
		// 	APP_DBG("[video] encode set config at channel %d err\n", i);
		// 	return -1;
		// }

		APP_DBG("+++ %s channel +++\n", (i == MTCE_MAIN_STREAM) ? "MAIN" : "SUB");
		APP_DBG("    - Encode          :	%s\n", (conf->format.compression == MTCE_CAPTURE_COMP_H264) ? "H264" : "H265");
		APP_DBG("    - Bitrate control :	%s\n", (conf->format.bitRateControl == MTCE_CAPTURE_BRC_VBR) ? "CBR" : "VBR");
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



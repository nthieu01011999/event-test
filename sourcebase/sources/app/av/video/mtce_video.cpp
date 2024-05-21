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

extern "C" VVTK_RET_CALLBACK onStartMainChannel(const vvtk_video_frame_t *videoFrame, const void *arg);
extern "C" VVTK_RET_CALLBACK onStartSubChannel(const vvtk_video_frame_t *videoFrame, const void *arg);
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

int VideoChannel::applyConf(int channel, const mtce_mediaFormat_t *mediaFormat) {
	int err, gopCal;
	vvtk_video_config_t videoConf, tmpConf;
	VVTK_VIDEO_ENCODING_MODE modeSet;
	VVTK_VIDEO_CODEC codecSet;

	if (mediaFormat->format.compression == MTCE_CAPTURE_COMP_H264)
		codecSet = VVTK_VIDEO_CODEC_H264;
	else
		codecSet = VVTK_VIDEO_CODEC_H265;

	if (mediaFormat->format.bitRateControl == MTCE_CAPTURE_BRC_CBR)
		modeSet = VVTK_VIDEO_ENCODING_MODE_CBR;
	else if (mediaFormat->format.bitRateControl == MTCE_CAPTURE_BRC_VBR)
		modeSet = VVTK_VIDEO_ENCODING_MODE_VBR;
	else
		modeSet = VVTK_VIDEO_ENCODING_MODE_CVBR;

	err = vvtk_get_video_config(channel, &videoConf);
	if (err != 0) {
		APP_DBG("[video] get encode config channel %d err: %d\n", channel, err);
		return -1;
	}
	else {
		APP_DBG("[vvtk_get_video_config] PASSED %d err: %d\n", channel, err);
	}
	videoConf.encoding_mode = modeSet;
	if (videoConf.encoding_mode == VVTK_VIDEO_ENCODING_MODE_VBR) {
		videoConf.vbr_quality = qualityMaps(mediaFormat->format.quality);
	}
	else if (videoConf.encoding_mode == VVTK_VIDEO_ENCODING_MODE_CBR) {
		videoConf.bitrate_max = mediaFormat->format.bitRate * 1024;
	}
	else {
		videoConf.bitrate_max = mediaFormat->format.bitRate * 1024;
		videoConf.bitrate_min = videoConf.bitrate_max * 2 / 3;
	}
	videoConf.codec		 = codecSet;
	videoConf.frame_rate = mediaFormat->format.FPS;
	gopCal = mediaFormat->format.GOP * mediaFormat->format.FPS;
	videoConf.gop		 = gopCal > MTCE_MAX_GOP ? MTCE_MAX_GOP : gopCal;
	videoConf.width		 = mWidth;
	videoConf.height	 = mHeight;
	err					 = vvtk_set_video_config(channel, &videoConf);
	if (err != 0) {
		APP_DBG("[video] encode set config channel %d err: %d\n", channel, err);
		return -1;
	}
	else {
		APP_DBG("Video Configuration Details: after run (vvtk_set_video_config) \n");
		APP_DBG("Codec			: 	%d\n", videoConf.codec);
		APP_DBG("Encoding Mode		: 	%d\n", videoConf.encoding_mode);
		APP_DBG("Width			: 	%d pixels\n", videoConf.width);
		APP_DBG("Height		: 	%d pixels\n", videoConf.height);
		APP_DBG("Frame Rate		: 	%d FPS\n", videoConf.frame_rate);
		APP_DBG("GOP Size		: 	%d\n", videoConf.gop);

		APP_DBG("Min Bitrate		: 	%d Kbps\n", videoConf.bitrate_min);
		APP_DBG("Max Bitrate		: 	%d Kbps\n", videoConf.bitrate_max);
		APP_DBG("VBR Quality Level	: 	%d\n", videoConf.vbr_quality);
	}

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
	VVTK_VIDEO_CALLBACK vcb = (channel == MTCE_MAIN_STREAM ? onStartMainChannel : onStartSubChannel);
	if (vvtk_set_video_callback(channel, vcb, this) == VVTK_RET_SUCCESS) {
		APP_DBG("[video] start stream on channel %d ok\n", channel);
		streamIsRunning = true;
	}
	else {
		APP_DBG("[video] start stream on channel %d error\n", channel);
	}

	return 0;
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
	videoForceStopStream = false;
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
    
    
	// 	/*apply config*/
		err = mVideoChn[i].applyConf(i, conf);
		// if (err != 0) {
		// 	APP_DBG("[video] encode set config at channel %d err\n", i);
		// 	return -1;
		// }
		APP_DBG("[VideoCtrl] READING FROM applyConf \n");
		APP_DBG("----------------------------------------------------------------\n");
		APP_DBG("+++ %s channel +++\n", (i == MTCE_MAIN_STREAM) ? "MAIN" : "SUB");
		APP_DBG("    - Encode          :	%s\n", (conf->format.compression == MTCE_CAPTURE_COMP_H264) ? "H264" : "H265");
		APP_DBG("    - Bitrate control :	%s\n", (conf->format.bitRateControl == MTCE_CAPTURE_BRC_VBR) ? "CBR" : "VBR");
		APP_DBG("    - Resolution      :	%d\n", conf->format.resolution);
		APP_DBG("    - FPS             :	%d\n", conf->format.FPS);
		APP_DBG("    - GOP             :	%d\n", conf->format.GOP);
		APP_DBG("    - Bitreate        :	%d\n", conf->format.bitRate);
		APP_DBG("    - Quality         :	%d\n", conf->format.quality);
		APP_DBG("----------------------------------------------------------------\n");
		APP_DBG("\n");
	}
	return 0;
}

// Start streams on all configured channels
void VideoCtrl::startStreamAllChannels() {
    std::cout << "Attempting to start all video streams." << std::endl;
    for (int i = 0; i < MTCE_MAX_STREAM_NUM; ++i) { // Assuming 4 channels for simplicity
        if (!mVideoChn[i].streamIsRunning) {
            mVideoChn[i].startStream(i);
        }
    }
	APP_PRINT("start stream done\n");
	videoForceStopStream = false;
    setInitialized(true);
}

// Stop streams on all channels
void VideoCtrl::stopStreamAllChannels() {
    std::cout << "Attempting to stop all video streams." << std::endl;
    for (int i = 0; i < MTCE_MAX_STREAM_NUM; ++i) {
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


int VideoCtrl::verifyConfig(mtce_encode_t *encodeConf) {
	mtce_mediaFormat_t *conf;
	int err = 0;
	for (int i = 0; i < MTCE_MAX_STREAM_NUM && err == 0; i++) {
		err	 = -1;
		conf = (mtce_mediaFormat_t *)encodeConf + i;
		if (i == 0 && (conf->format.resolution >= MTCE_CAPTURE_SIZE_NONE || conf->format.resolution < MTCE_CONFIG_NONE)) {
			APP_DBG("[Video] encode error resolution [%d] channel %d\n", conf->format.resolution, i);
		}
		else if (i == 1 && (conf->format.resolution > MTCE_CAPTURE_SIZE_720P || conf->format.resolution < MTCE_CONFIG_NONE)) {
			APP_DBG("[Video] encode error resolution [%d] channel %d\n", conf->format.resolution, i);
		}
		else if (conf->format.bitRateControl >= MTCE_CAPTURE_BRC_NR || conf->format.bitRateControl < MTCE_CONFIG_NONE) {
			APP_DBG("[Video] encode error bitrate control [%d] channel %d\n", conf->format.bitRateControl, i);
		}
		else if (conf->format.compression >= MTCE_CAPTURE_COMP_NONE || conf->format.compression < MTCE_CONFIG_NONE) {
			APP_DBG("[video] encode error compression [%d] channel %d\n", conf->format.compression, i);
		}
		else if (conf->format.bitRate > MTCE_MAX_BITRATE || conf->format.bitRate <= MTCE_CONFIG_NONE) {
			APP_DBG("[video] encode error bitrate [%d] channel %d\n", conf->format.bitRate, i);
		}
		else if (conf->format.FPS > MTCE_MAX_FPS || conf->format.FPS <= MTCE_CONFIG_NONE) {
			APP_DBG("[video] encode error fps [%d] channel %d\n", conf->format.FPS, i);
		}
		else if (conf->format.GOP > MTCE_MAX_GOP || conf->format.GOP <= MTCE_CONFIG_NONE) {
			APP_DBG("[video] encode error gop [%d] channel %d\n", conf->format.GOP, i);
		}
		else if (conf->format.quality > MTCE_MAX_QUALITY || conf->format.quality <= MTCE_CONFIG_NONE) {
			APP_DBG("[video] encode error quality [%d] channel %d\n", conf->format.quality, i);
		}
		else {
			APP_DBG("[video] Verify config encode channel %d success\n", i);
			err = 0;
		}
	}

	return err;
}

void VideoCtrl::setVideoEncodeConfig(const mtce_encode_t *newEncodeChannels) {
 	APP_DBG("[setVideoEncodeConfig] ==========================================\n");
     memcpy(&mEncodeConfig, newEncodeChannels, sizeof(mEncodeConfig));

     int verificationResult = verifyConfig(&mEncodeConfig);
    if (verificationResult != APP_CONFIG_SUCCESS) {
        APP_DBG("[video] Verification of new encode config failed\n");
        return;
    }

     for (int i = 0; i < MTCE_MAX_STREAM_NUM; i++) {
        mtce_mediaFormat_t *conf;
        if (i == MTCE_MAIN_STREAM) {
            conf = &mEncodeConfig.mainFmt;
        } else if (i == MTCE_SUB_STREAM) {
            conf = &mEncodeConfig.extraFmt;
        } else {
            APP_DBG("[video] Invalid stream index: %d\n", i);
            continue;
        }
        int applyResult = mVideoChn[i].applyConf(i, conf);
        if (applyResult != APP_CONFIG_SUCCESS) {
            APP_DBG("[video] Failed to apply config to channel %d\n", i);
        }
    }

    // Step 5: Log the new configuration for debugging
    printEncodeConfig();
}

void VideoCtrl::printEncodeConfig() {
	APP_DBG("[VAR] --------------------- Main channel ---------------------\n");
	APP_DBG("[VAR] AudioEnable: %d\n", mEncodeConfig.mainFmt.bAudioEnable);
	APP_DBG("[VAR] Compression: %d\n", mEncodeConfig.mainFmt.format.compression);
	APP_DBG("[VAR] Resolution: %d\n", mEncodeConfig.mainFmt.format.resolution);
	APP_DBG("[VAR] BitRate: %d\n", mEncodeConfig.mainFmt.format.bitRate);
	APP_DBG("[VAR] VirtualGOP: %d\n", mEncodeConfig.mainFmt.format.virtualGOP);
	APP_DBG("[VAR] BitRateControl: %d\n", mEncodeConfig.mainFmt.format.bitRateControl);
	APP_DBG("[VAR] FPS: %d\n", mEncodeConfig.mainFmt.format.FPS);
	APP_DBG("[VAR] Quality: %d\n", mEncodeConfig.mainFmt.format.quality);
	APP_DBG("[VAR] GOP: %d\n", mEncodeConfig.mainFmt.format.GOP);
	APP_DBG("[VAR] VideoEnable: %d\n", mEncodeConfig.mainFmt.bVideoEnable);
	APP_DBG("[VAR] --------------------- Sub channel ---------------------\n");
	APP_DBG("[VAR] AudioEnable: %d\n", mEncodeConfig.extraFmt.bAudioEnable);
	APP_DBG("[VAR] Compression: %d\n", mEncodeConfig.extraFmt.format.compression);
	APP_DBG("[VAR] Resolution: %d\n", mEncodeConfig.extraFmt.format.resolution);
	APP_DBG("[VAR] BitRate: %d\n", mEncodeConfig.extraFmt.format.bitRate);
	APP_DBG("[VAR] VirtualGOP: %d\n", mEncodeConfig.extraFmt.format.virtualGOP);
	APP_DBG("[VAR] BitRateControl: %d\n", mEncodeConfig.extraFmt.format.bitRateControl);
	APP_DBG("[VAR] FPS: %d\n", mEncodeConfig.extraFmt.format.FPS);
	APP_DBG("[VAR] Quality: %d\n", mEncodeConfig.extraFmt.format.quality);
	APP_DBG("[VAR] GOP: %d\n", mEncodeConfig.extraFmt.format.GOP);
	APP_DBG("[VAR] VideoEnable: %d\n", mEncodeConfig.extraFmt.bVideoEnable);
}

VVTK_RET_CALLBACK onStopMainChannel(const vvtk_video_frame_t *videoFrame, const void *arg) {
	(void)videoFrame;
	(void)arg;
	APP_DBG("onStopMainChannel callback\n");
	int ret = (int)VVTK_RET_CALLBACK_STOP;
	task_post_dynamic_msg(GW_TASK_AV_ID, GW_AV_SET_ENCODE_STOP_VIDEO_MAIN_STREAM_RES, (uint8_t *)&ret, sizeof(ret));
	return VVTK_RET_CALLBACK_STOP;
}

VVTK_RET_CALLBACK onStopSubChannel(const vvtk_video_frame_t *videoFrame, const void *arg) {
	(void)videoFrame;
	(void)arg;
	APP_DBG("onStopSubChannel callback\n");
	int ret = (int)VVTK_RET_CALLBACK_STOP;
	task_post_dynamic_msg(GW_TASK_AV_ID, GW_AV_SET_ENCODE_STOP_VIDEO_SUB_STREAM_RES, (uint8_t *)&ret, sizeof(ret));
	return VVTK_RET_CALLBACK_STOP;
}

extern "C" VVTK_RET_CALLBACK onStartMainChannel(const vvtk_video_frame_t *videoFrame, const void *arg) {
	if (videoFrame->size > (int)sizeof(rtc::NalUnitHeader)) {
		onSampleVideoCapture(MTCE_MAIN_STREAM, videoFrame->data, videoFrame->size);
	}

	return VVTK_RET_CALLBACK_CONTINUE;
}

extern "C" VVTK_RET_CALLBACK onStartSubChannel(const vvtk_video_frame_t *videoFrame, const void *arg) {
	if (videoFrame->size > (int)sizeof(rtc::NalUnitHeader)) {
		onSampleVideoCapture(MTCE_SUB_STREAM, videoFrame->data, videoFrame->size);
	}

	return VVTK_RET_CALLBACK_CONTINUE;
}
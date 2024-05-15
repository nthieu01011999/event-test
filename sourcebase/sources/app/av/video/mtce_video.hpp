#ifndef __VIDEO_HPP__
#define __VIDEO_HPP__

#include <pthread.h>
#include <vvtk_video.h>
#include <vvtk_def.h>

#include "app_data.h"
#include "app_dbg.h"
#include "app_config.h"
#include "parser_json.h"

#define MTCE_MAIN_STREAM	0
#define MTCE_SUB_STREAM		1
#define MTCE_MAX_STREAM_NUM 2

extern "C" VVTK_RET_CALLBACK onStartMainChannel(const vvtk_video_frame_t *videoFrame, const void *arg);
extern "C" VVTK_RET_CALLBACK onStartSubChannel(const vvtk_video_frame_t *videoFrame, const void *arg);

class VideoCtrl {
public:
	VideoCtrl();
	~VideoCtrl();

	bool streamIsRunning;
	bool initializeCamera(int channel);
	void startCapture(int channel);
	void stopCapture(int channel);\
	void captureFrame(int channel, uint8_t *bytes, uint32_t nbBytes);
	bool getStreamIsRunningChannel(int channel);
	void stopStreamAllChannels();
	std::atomic<bool> videoForceStopStream;

private:
    // std::optional<std::shared_ptr<VideoCtrl>> mVideoChn[MTCE_MAX_STREAM_NUM];
    std::atomic<bool> mInitialized;
};
 
#endif	  // __MTCE_VIDEO_H__















 
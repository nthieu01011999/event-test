#ifndef __VIDEO_HPP__
#define __VIDEO_HPP__

#include <atomic>

// Basic structure for video resolution
typedef struct {
    int width;
    int height;
} mtce_sizePicture_t;

// Basic video channel class for handling video streams
class VideoChannel {
public:
    VideoChannel();
    ~VideoChannel();

    void setConfChannel(const mtce_mediaFormat_t *mediaFormat);
    int startStream(int channel);
    void stopStream(int channel);
    bool streamIsRunning;
    int applyConf(int channel, const mtce_mediaFormat_t *mediaFormat);

protected:
	void diffSetAndGetConf(vvtk_video_config_t *videoConf, vvtk_video_config_t *tmpConf);
	VVTK_VBR_QUALITY qualityMaps(int mode);
    
private:
    int mWidth;
    int mHeight;
};

// Controller class for managing video channels
class VideoCtrl {
public:
    VideoCtrl();
    ~VideoCtrl();
    int loadConfigFromFileChannels(mtce_encode_t *encodeConf);
    void startStreamAllChannels();
    void stopStreamAllChannels();

    bool initialized() const;
    void setInitialized(bool newInitialized);
    int setVideoEncodeChannels(mtce_encode_t *encodeConf);
    int verifyConfig(mtce_encode_t *encodeConf);
	void setVideoEncodeConfig(const mtce_encode_t *newEncodeChannels);
    void printEncodeConfig();

	std::atomic<bool> videoForceStopStream;
private:
	VideoChannel mVideoChn[MTCE_MAX_STREAM_NUM];
	mtce_encode_t mEncodeConfig;
	std::atomic<bool> mInitialized;
};

#endif  // __VIDEO_HPP__

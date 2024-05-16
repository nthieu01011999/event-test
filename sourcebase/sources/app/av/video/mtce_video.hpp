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
private:
    int mWidth;
    int mHeight;
};

// Controller class for managing video channels
class VideoCtrl {
public:
    VideoCtrl();
    ~VideoCtrl();

    void startStreamAllChannels();
    void stopStreamAllChannels();

    bool initialized() const;
    void setInitialized(bool newInitialized);
    int setVideoEncodeChannels(mtce_encode_t *encodeConf);
private:
    std::atomic<bool> mInitialized;
    VideoChannel mVideoChn[4];  // Assuming a maximum of 4 channels for simplicity
};

#endif  // __VIDEO_HPP__

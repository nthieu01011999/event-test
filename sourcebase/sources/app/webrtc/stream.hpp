#ifndef __STREAM_H
#define __STREAM_H

#include <pthread.h>
#include <optional>
#include "streamsource.hpp"

#define LIVE_VIDEO_FPS			(13)
#define LIVE_AUDIO_SPS			(47)
#define PLAYBACK_VIDEO_FPS		(15)
#define PLAYBACK_AUDIO_SPS		(50)

class Stream : public std::enable_shared_from_this<Stream> {
public:
	Stream(std::shared_ptr<MediaStream> mediaLive, std::shared_ptr<MediaStream> mediaPLayback);
	~Stream();
    void start();
    void stop();
	void triggerPbSession();
	void pendingPbSession();
	void onPbSampleHdl(std::function<void(StreamSourceType, uint64_t)> handler);

public:
    std::shared_ptr<MediaStream> mediaLive;
	std::shared_ptr<MediaStream> mediaPLayback;

	const bool& isPbRunning = mIsPbRunning;

private:
	pthread_mutex_t mPOSIXMutex;
	pthread_cond_t mPOSIXCond;
	pthread_t mPbSampleThreadId;

    bool mIsStarted = false;
	bool mIsPbRunning = false;

	rtc::synchronized_callback<StreamSourceType, uint64_t> mPbSampleHdl;

	std::pair<std::shared_ptr<StreamSource>, StreamSourceType> unsafePrepareForSample();
	static void* sendPbSample(void*);

public:
    static pthread_mutex_t pubLicStreamPOSIXMutex;

	static std::vector<std::byte> nalUnitsMain;
	static std::vector<std::byte> nalUnitsSub;

	static void pubLicStreamPOSIXMutexLOCK();
	static void pubLicStreamPOSIXMutexUNLOCK();
	static void MediaLiveVideo(bool isFullHD, uint8_t *, uint32_t);
	static void MediaLiveAudio(uint8_t *, uint32_t);
};

 
#endif /* __STREAM_H */

/*

Payload Size: 256 bytes.

G.711 A-law Sampling Rate: G.711 A-law typically operates at a sampling rate of 8,000 Hz.

To calculate the sample duration::

Sample Duration (seconds) 	= Payload Size (bytes) / Sampling Rate (sps)
							= 256 / 8000
							= 0.032 (seconds)
							= 32000 (Microseconds)

LIVE_AUDIO_SPS 	= 1000 * 1000 / Sample Duration (seconds)
				= 1000 * 1000 / 32000
				= 31.25 ~ 32
*/

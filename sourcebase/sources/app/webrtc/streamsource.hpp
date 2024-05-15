#ifndef __STREAM_SOURCE_H
#define __STREAM_SOURCE_H

#include "rtc/rtc.hpp"

enum StreamSourceType {
	Audio,
	Video
};

class LiveStream {
public:
	enum eResolution {
		HD720p,
		FullHD1080p,
	};
};

class PlayBack {
public:
	enum ePbControl {
		Play,
		Pause,
		Stop,
		Resume,
		Seek,
		Speed,
	};

	enum ePbStatus {
		Playing,
		Pausing,
		Stopped,
		Resuming,
		Seeking,
		Speeding,
		Done,
		Error
	};

	enum class ePbSpeedLoad {
		Slow,
		Normal,
		FastX2,
		FastX4,
		FastX8,
		FastX16,
	};
};

typedef struct {
	/* Record query attributes */
	std::string url;
	size_t track;
	size_t fileSize;
	uint32_t durationInSec = 0;
	PlayBack::ePbSpeedLoad loadSpeed;
	uint32_t lastSecondsTick;
	int8_t nbrNalusLoad;

	/* Timestamp for each client in playback mode */
	uint64_t sampleDuration_us;
	uint64_t sampleTime_us = 0;
} SDSource;

class StreamSource {
protected:
	rtc::binary sample;

public:
	virtual void start()							 = 0;
	virtual void stop()								 = 0;
	virtual void loadNextTime()						 = 0;
	virtual void loadNextSample(SDSource*) 			 = 0;
	virtual void loadNextSample(uint8_t *, uint32_t) = 0;
	virtual uint64_t getSampleTime_us()				 = 0;
	virtual uint64_t getSampleDuration_us()			 = 0;
	virtual rtc::binary& getSample()				 = 0;
	void rstSample() {
		sample.clear();
		sample.shrink_to_fit();
	}
};

struct MediaStream {
	std::shared_ptr<StreamSource> video;
	std::shared_ptr<StreamSource> audio;
	uint64_t startTime;

	MediaStream(std::shared_ptr<StreamSource> video, std::shared_ptr<StreamSource> audio);
	void start();
	void stop();
	void loadNextSample(SDSource *sd, StreamSourceType type);
	void loadNextSample(uint8_t *, uint32_t, StreamSourceType type);
	rtc::binary& getSample(StreamSourceType type);
	void rstSample(StreamSourceType type);
	uint64_t getSampleTime_us(StreamSourceType type);
    uint64_t getSampleDuration_us(StreamSourceType type);
};

extern uint64_t currentTimeInMicroSeconds();

#endif /* __STREAM_SOURCE_H */
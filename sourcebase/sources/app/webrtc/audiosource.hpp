#ifndef __AUDIO_SOURCE_H__
#define __AUDIO_SOURCE_H__

#include <optional>
#include <string>
#include <vector>
#include <arpa/inet.h>

#include "streamsource.hpp"

class AudioSource : public StreamSource {
protected:
	uint64_t sampleDuration_us;
	uint64_t sampleTime_us = 0;

public:
	AudioSource(uint32_t sps);
	~AudioSource();
	void start();
	void stop();
	void loadNextTime();
	void loadNextSample(SDSource*);
	void loadNextSample(uint8_t *, uint32_t);
	rtc::binary& getSample();
	uint64_t getSampleTime_us();
	uint64_t getSampleDuration_us();
};

#endif /* __AUDIO_SOURCE_H__ */

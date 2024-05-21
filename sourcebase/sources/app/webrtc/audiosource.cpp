#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "audiosource.hpp"
#include "helpers.hpp"

using namespace std;

static size_t loadSampleTrack(SDSource *sd, std::byte *p, uint32_t nbrOfBytes) {
	int fd = -1;

	if (sd->track >= sd->fileSize) {
		return 0;
	}

	fd = open(sd->url.c_str(), O_RDONLY);
	if (fd < 0) {
		return 0;
	}

	size_t remain	 = sd->fileSize - sd->track;
	size_t validSize = (remain > nbrOfBytes ? nbrOfBytes : remain);
	size_t ret		 = pread(fd, p, validSize, sd->track);

	close(fd);

	return ret;
}

AudioSource::AudioSource(uint32_t sps) {
	this->sampleDuration_us = 1000 * 1000 / sps;
}

AudioSource::~AudioSource() {
	stop();
}

void AudioSource::start() {							
	sampleTime_us = std::numeric_limits<uint64_t>::max() - sampleDuration_us + 1;
	loadNextTime();
}

void AudioSource::stop() {
	StreamSource::rstSample();
	sampleTime_us = 0;
}

void AudioSource::loadNextTime() {
	sampleTime_us += sampleDuration_us;
}

void AudioSource::loadNextSample(SDSource *sd) {
#define AUDIO_SAMPLES_LOAD_MAX	(160)

	std::byte *bytes = NULL;

	if (sd->loadSpeed == PlayBack::ePbSpeedLoad::Slow) {
		if (sd->nbrNalusLoad < 0) {
			sd->nbrNalusLoad = 1;
			return;
		}
	}

	bytes = (std::byte *)malloc(sizeof(std::byte) * AUDIO_SAMPLES_LOAD_MAX);

	if (bytes == NULL) {
		return;
	}

	for (uint16_t cnts = 0; cnts < sd->nbrNalusLoad; ++cnts) {
		memset(bytes, 0, AUDIO_SAMPLES_LOAD_MAX);
		int totalSamples = loadSampleTrack(sd, bytes, AUDIO_SAMPLES_LOAD_MAX);
		if (totalSamples != 0) {
			sample.insert(sample.end(), bytes, bytes + totalSamples);
			sd->track += totalSamples;
		}
	}

	free(bytes);

	if (sd->loadSpeed == PlayBack::ePbSpeedLoad::Slow) {
		sd->nbrNalusLoad = -1;
	}
}

void AudioSource::loadNextSample(uint8_t *bytes, uint32_t len) {
	sample.insert(sample.end(), 
				  reinterpret_cast<std::byte *>(bytes), 
				  reinterpret_cast<std::byte *>(bytes) + len);
}

rtc::binary& AudioSource::getSample() {
	return sample;
}

uint64_t AudioSource::getSampleTime_us() {
	return sampleTime_us;
}

uint64_t AudioSource::getSampleDuration_us() {
	return sampleDuration_us;
}

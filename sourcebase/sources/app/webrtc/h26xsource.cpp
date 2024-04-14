#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

#include "h26xsource.hpp"
#include "utils.h"

using namespace std;

static size_t loadSampleTrack(SDSource *sd, std::byte *p, uint32_t nbrOfBytes) {
	int fd = -1;
	size_t remain,validSize, ret = 0;
	
	if (sd->track >= sd->fileSize) {
		return 0;
	}

	fd = open(sd->url.c_str(), O_RDONLY);
	if (fd < 0) {
		return 0;
	}
	
	try {
		remain	 = sd->fileSize - sd->track;
		validSize = (remain > nbrOfBytes ? nbrOfBytes : remain);
		ret		 = pread(fd, p, validSize, sd->track);
	}
	catch(const exception &e) {
		std::cout << e.what() << std::endl;
	}

	close(fd);

	return ret;
}


H26XSource::H26XSource(uint32_t fps) {
	this->sampleDuration_us = 1000 * 1000 / fps;
}

H26XSource::~H26XSource() {
	stop();
}

void H26XSource::start() {
	sampleTime_us = std::numeric_limits<uint64_t>::max() - sampleDuration_us + 1;
	loadNextTime();
}

void H26XSource::stop() {
	StreamSource::rstSample();
	sampleTime_us = 0;
}

void H26XSource::loadNextTime() {	
	sampleTime_us += sampleDuration_us;
}

void H26XSource::loadNextSample(SDSource *sd) {
	enum {
		NALU_NOTFOUND = 0,
		NALU_BEGIN,
		NALU_END,
	};

	std::vector<std::byte> tmp;
	size_t beginNalu = NALU_NOTFOUND;
	bool isFound	 = false;
	std::byte *bytes = NULL;

	if (sd->loadSpeed == PlayBack::ePbSpeedLoad::Slow) {
		if (sd->nbrNalusLoad < 0) {
			sd->nbrNalusLoad = 1;
			return;
		}
	}

	bytes = (std::byte *)malloc(sizeof(std::byte) * 2048);
	if (bytes == NULL) {
		return;
	}

	for (uint16_t ind = 0; ind < sd->nbrNalusLoad; ++ind) {
		beginNalu = NALU_NOTFOUND;
		isFound	  = false;

		do {
			memset(bytes, 0, 2048);
			size_t totalSamples = loadSampleTrack(sd, bytes, 2048);
			if (totalSamples <= 4) {
				sd->track += totalSamples;
				break;
			}

			for (size_t id = 0; id < (totalSamples - 4); ++id) {
				if (isNALU4(id, bytes) == true) {
					++beginNalu;
				}

				if (beginNalu == NALU_NOTFOUND) {
					++(sd->track);
					continue;
				}
				else if (beginNalu == NALU_BEGIN) {
					tmp.emplace_back(reinterpret_cast<const std::byte>(bytes[id]));
					++(sd->track);
				}
				else {
					isFound = true;
					break;
				}
			}
		} while (!isFound);

		uint32_t naluLen = htonl(static_cast<uint32_t>(tmp.size() - 4));
		std::copy(reinterpret_cast<const std::byte *>(&naluLen), reinterpret_cast<const std::byte *>(&naluLen) + sizeof(naluLen), tmp.begin());
		sample.insert(sample.end(), tmp.begin(), tmp.end());
		tmp.clear();
	}

	free(bytes);

	if (sd->loadSpeed == PlayBack::ePbSpeedLoad::Slow) {
		sd->nbrNalusLoad = -1;
	}
}

void H26XSource::loadNextSample(uint8_t *bytes, uint32_t len) {
	splitNALUS((unsigned char *)bytes, len);
	extractNALUS(sample);
}

rtc::binary& H26XSource::getSample() {
	return sample;
}

uint64_t H26XSource::getSampleTime_us() {
	return sampleTime_us;
}

uint64_t H26XSource::getSampleDuration_us() {
	return sampleDuration_us;
}

vector<byte> H26XSource::initialNALUS() {
	uint8_t unitTypeCnts = 0;
	vector<byte> units{};
	
	if (previousUnitType7.has_value()) {
		auto nalu = previousUnitType7.value();
		units.insert(units.end(), nalu.begin(), nalu.end());
		
		++unitTypeCnts; /* SPS Frame has found */
	}
	if (previousUnitType8.has_value()) {
		auto nalu = previousUnitType8.value();
		units.insert(units.end(), nalu.begin(), nalu.end());
		
		++unitTypeCnts; /* PPS Frame has found */
	}
	if (previousUnitType5.has_value()) {
		auto nalu = previousUnitType5.value();
		units.insert(units.end(), nalu.begin(), nalu.end());
		
		++unitTypeCnts; /* IDR Frame has found */
	}

	if (unitTypeCnts != 3) {
		units.clear();
	}

	return units;
}

std::vector<std::byte> H26XSource::initialNALUS(const char *url, bool isCaptureThumbnail) {
	std::vector<std::byte> idr, sps, pps;
	std::vector<std::byte> units;

	SDSource sd;

	memset(&sd, 0, sizeof(SDSource));
	sd.url = std::string(url, strlen(url));
	sd.nbrNalusLoad = 2;
	sd.fileSize = sizeOfFile(sd.url.c_str());

	if (sd.fileSize < 0) {
		return units;
	}

	auto h26XSource = std::make_shared<H26XSource>(PLAYBACK_VIDEO_FPS);

	do {
		h26XSource->loadNextSample(&sd);
		auto sample = h26XSource->getSample();

		h26XSource->extractNALUS(sample);

		if (h26XSource->previousUnitType7.has_value()) { /* SPS Frame has found */
			sps = h26XSource->previousUnitType7.value();
		}
		if (h26XSource->previousUnitType8.has_value()) { /* PPS Frame has found */
			pps = h26XSource->previousUnitType8.value();
		}
		if (h26XSource->previousUnitType5.has_value()) { /* IDR Frame has found */
			idr = h26XSource->previousUnitType5.value();
		}

		if (sd.track >= sd.fileSize) {
			break;
		}
	} while (!idr.size() || !sps.size() || !pps.size());

	if (idr.size() && sps.size() && pps.size()) {
		if (isCaptureThumbnail) {
			idr[0] = idr[1] = idr[2] = std::byte{0}; idr[3] = std::byte{1};
			sps[0] = sps[1] = sps[2] = std::byte{0}; sps[3] = std::byte{1};
			pps[0] = pps[1] = pps[2] = std::byte{0}; pps[3] = std::byte{1};
		}
		units.insert(units.end(), sps.begin(), sps.end());
		units.insert(units.end(), pps.begin(), pps.end());
		units.insert(units.end(), idr.begin(), idr.end());
	}

	return units;
}

void H26XSource::appendSample(unsigned char *buffer, size_t length) {
	uint32_t nalu_nlen = htonl((uint32_t)length);
	sample.insert(sample.end(), reinterpret_cast<const byte *>(&nalu_nlen), reinterpret_cast<const byte *>(&nalu_nlen) + sizeof(nalu_nlen));
	sample.insert(sample.end(), reinterpret_cast<const byte *>(buffer), reinterpret_cast<const byte *>(buffer) + length);
}

void H26XSource::splitNALUS(unsigned char *buffer, size_t length) {
	const unsigned char zero_sequence_4[4] = {0, 0, 0, 1};
	unsigned char *begin				   = buffer;
	unsigned char *found				   = NULL;
	size_t remain_length				   = length;

	found = (unsigned char *)memmem(begin, remain_length, zero_sequence_4, sizeof(zero_sequence_4));
	while (found) {
		int nalu_hlen = found - begin;
		if (nalu_hlen) {
			appendSample(begin, nalu_hlen);
		}
		begin = found + sizeof(zero_sequence_4);
		remain_length -= (nalu_hlen + sizeof(zero_sequence_4));
		found = (unsigned char *)memmem(begin, remain_length, zero_sequence_4, sizeof(zero_sequence_4));
	}
	if (remain_length) {
		appendSample(begin, remain_length);
	}
}

void H26XSource::extractNALUS(rtc::binary &samples) {
	size_t i = 0;
	while (i < samples.size()) {
		assert(i + 4 < samples.size());
		auto lengthPtr		= (uint32_t *)(samples.data() + i);
		uint32_t length		= ntohl(*lengthPtr);
		auto naluStartIndex = i + 4;
		auto naluEndIndex	= naluStartIndex + length;
		assert(naluEndIndex <= samples.size());
		auto header = reinterpret_cast<rtc::NalUnitHeader *>(samples.data() + naluStartIndex);
		auto type	= header->unitType();

		switch (type) {
		case 7:
			previousUnitType7 = {samples.begin() + i, samples.begin() + naluEndIndex};
		break;
		case 8:
			previousUnitType8 = {samples.begin() + i, samples.begin() + naluEndIndex};
		break;
		case 5:
			previousUnitType5 = {samples.begin() + i, samples.begin() + naluEndIndex};
		break;
		}
		i = naluEndIndex;
	}
}

bool H26XSource::isNALU4(size_t startPos, std::byte *p) {
	return (p[startPos + 0] == std::byte{0} && p[startPos + 1] == std::byte{0} && p[startPos + 2] == std::byte{0} && p[startPos + 3] == std::byte{1});
}

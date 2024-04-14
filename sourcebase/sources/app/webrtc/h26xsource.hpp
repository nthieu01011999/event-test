#ifndef __H26x_SOURCE_H__
#define __H26x_SOURCE_H__

#include <optional>
#include <string>
#include <vector>
#include <arpa/inet.h>

#include "streamsource.hpp"

class H26XSource : public StreamSource {
private:

protected:
	uint64_t sampleDuration_us;
	uint64_t sampleTime_us									= 0;
	std::optional<std::vector<std::byte>> previousUnitType5 = std::nullopt;	/* Coded slice of an IDR picture 	*/
	std::optional<std::vector<std::byte>> previousUnitType7 = std::nullopt;	/* Sequence parameter set 			*/
	std::optional<std::vector<std::byte>> previousUnitType8 = std::nullopt; /* Picture parameter set 			*/

	void appendSample(unsigned char *buffer, size_t length);
	void splitNALUS(unsigned char *buffer, size_t length);
	void extractNALUS(rtc::binary &samples);
	bool isNALU4(size_t startPos, std::byte *p);

public:
	H26XSource(uint32_t fps);
	~H26XSource();
	void start();
	void stop();
	void loadNextTime();
	void loadNextSample(SDSource*);
	void loadNextSample(uint8_t *, uint32_t);
	rtc::binary& getSample();
	uint64_t getSampleTime_us();
	uint64_t getSampleDuration_us();
	std::vector<std::byte> initialNALUS();

	static std::vector<std::byte> initialNALUS(const char *url, bool isCaptureThumbnail = false);
};

#endif /* __H26x_SOURCE_H__ */

/*
Reference: http://yumichan.net/video-processing/video-compression/introduction-to-h264-nal-unit
*/
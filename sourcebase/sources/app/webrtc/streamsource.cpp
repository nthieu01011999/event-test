#include <unistd.h>
#include <ctime>
#include <arpa/inet.h>
#include <sys/time.h>

#include "streamsource.hpp"

uint64_t currentTimeInMicroSeconds() {
    struct timeval time;
	gettimeofday(&time, NULL);
	return uint64_t(time.tv_sec) * 1000 * 1000 + time.tv_usec;
}

MediaStream::MediaStream(std::shared_ptr<StreamSource> video, std::shared_ptr<StreamSource> audio) :
        video(video), audio(audio)
{

}

void MediaStream::start() {
	startTime = currentTimeInMicroSeconds();
	video->start();
	audio->start();
}

void MediaStream::stop() {
	video->stop();
	audio->stop();
}

void MediaStream::loadNextSample(SDSource *sd, StreamSourceType type) {
	return (type == StreamSourceType::Video) ? video->loadNextSample(sd) : audio->loadNextSample(sd);
}

void MediaStream::loadNextSample(uint8_t *bytes, uint32_t nbrOfBytes, StreamSourceType type) {
	return (type == StreamSourceType::Video) ? video->loadNextSample(bytes, nbrOfBytes) : audio->loadNextSample(bytes, nbrOfBytes);
}

rtc::binary& MediaStream::getSample(StreamSourceType type) {
	return (type == StreamSourceType::Video) ? video->getSample() : audio->getSample();
}

void MediaStream::rstSample(StreamSourceType type) {
	(type == StreamSourceType::Video) ? video->rstSample() : audio->rstSample();
}

uint64_t MediaStream::getSampleTime_us(StreamSourceType type) {
	return (type == StreamSourceType::Video) ? video->getSampleTime_us() : audio->getSampleTime_us();
}

uint64_t MediaStream::getSampleDuration_us(StreamSourceType type) {
	return (type == StreamSourceType::Video) ? video->getSampleDuration_us() : audio->getSampleDuration_us();
}

#include <unistd.h>
#include <ctime>
#include <arpa/inet.h>
#include <sys/time.h>
#include <chrono>

#include "stream.hpp"
#include "app_data.h"

/* =======================================================================================
 *	S T R E A M			C O N S T R U C T O R
 * =======================================================================================
 * Methods:
 * 		Stream
 * 		~Stream
 *-----------------------------------------------------------------------------------------
 */
Stream::Stream(std::shared_ptr<MediaStream> mediaLive, std::shared_ptr<MediaStream> mediaPLayback)
	: std::enable_shared_from_this<Stream>(), mediaLive(mediaLive), mediaPLayback(mediaPLayback) {
	mPOSIXMutex = PTHREAD_MUTEX_INITIALIZER;
	mPOSIXCond	= PTHREAD_COND_INITIALIZER;
}

Stream::~Stream() {
	pthread_mutex_destroy(&mPOSIXMutex);
	pthread_cond_destroy(&mPOSIXCond);

	std::cout << "[DECONSTRUCTOR] Media stream has coslaped" << std::endl;
}

/* =======================================================================================
 *	S T R E A M			M E T H O D S
 * =======================================================================================
 * Properties:
 * 		mPOSIXMutex
 * 		mPOSIXCond
 * 		mPbSampleThreadId
 * 		mIsStarted
 * 		mIsPbRunning
 * 		mPbSampleHdl
 * Methods:
 * 		unsafePrepareForSample
 * 		sendPbSample
 * 		start
 *		stop
 *		triggerPbSession
 *		pendingPbSession
 * 		onPbSampleHdl
 *-----------------------------------------------------------------------------------------
 */
std::pair<std::shared_ptr<StreamSource>, StreamSourceType> Stream::unsafePrepareForSample() {
	std::shared_ptr<StreamSource> ss;
	StreamSourceType sst;
	uint64_t nextTime;

	if (mediaPLayback->getSampleTime_us(StreamSourceType::Audio) < mediaPLayback->getSampleTime_us(StreamSourceType::Video)) {
		ss	= mediaPLayback->audio;
		sst = StreamSourceType::Audio;
	}
	else {
		ss	= mediaPLayback->video;
		sst = StreamSourceType::Video;
	}
	nextTime = mediaPLayback->getSampleTime_us(sst);

	auto currentTime = currentTimeInMicroSeconds();
	auto elapsed	 = currentTime - mediaPLayback->startTime;

	if (nextTime > elapsed) {
		auto waitTime = nextTime - elapsed;
		pthread_mutex_unlock(&mPOSIXMutex);
		usleep(waitTime);
		pthread_mutex_lock(&mPOSIXMutex);
	}

	return {ss, sst};
}

void *Stream::sendPbSample(void *arg) {
	Stream *argStream = static_cast<Stream *>(arg);

	while (argStream->mIsStarted) {
		pthread_mutex_lock(&argStream->mPOSIXMutex);

		if (argStream->mIsPbRunning == false) {
			pthread_cond_wait(&argStream->mPOSIXCond, &argStream->mPOSIXMutex);
		}

		auto ssSST = argStream->unsafePrepareForSample();
		auto ss	   = ssSST.first;
		auto sst   = ssSST.second;

		argStream->mPbSampleHdl(sst, ss->getSampleTime_us());
		ss->loadNextTime();

		pthread_mutex_unlock(&argStream->mPOSIXMutex);
	}

	return NULL;
}

void Stream::start() {
	pthread_mutex_lock(&mPOSIXMutex);

	if (mIsStarted) {
		pthread_mutex_unlock(&mPOSIXMutex);
		return;
	}

	mediaLive->start();
	mIsStarted = true;

	pthread_mutex_unlock(&mPOSIXMutex);

	pthread_create(&mPbSampleThreadId, NULL, sendPbSample, this);
}

void Stream::stop() {
	pthread_mutex_lock(&mPOSIXMutex);

	if (!mIsStarted) {
		pthread_mutex_unlock(&mPOSIXMutex);
		return;
	}

	mediaLive->stop();
	mediaPLayback->stop();
	mIsStarted	 = false;
	mIsPbRunning = false;

	pthread_mutex_unlock(&mPOSIXMutex);

	pthread_cond_signal(&mPOSIXCond);	   /* Condition trigger to exit */
	pthread_join(mPbSampleThreadId, NULL); /* Waiting for thread completely escaped */
};

void Stream::triggerPbSession() {
	pthread_mutex_lock(&mPOSIXMutex);

	if (mIsPbRunning == false) {
		mIsPbRunning = true;
		mediaPLayback->start();
	}

	pthread_mutex_unlock(&mPOSIXMutex);

	pthread_cond_signal(&mPOSIXCond);
}

void Stream::pendingPbSession() {
	mIsPbRunning = false;
	mediaPLayback->stop();
}

void Stream::onPbSampleHdl(std::function<void(StreamSourceType, uint64_t)> handler) {
	mPbSampleHdl = handler;
}

/* =======================================================================================
 *	S T A T I C		S T R E A M			I M P L E M E N T A T I O N
 * =======================================================================================
 * Properties:
 * 		pubLicStreamPOSIXMutex
 * Methods:
 * 		pubLicStreamPOSIXMutexLOCK
 * 		pubLicStreamPOSIXMutexUNLOCK
 * 		syncLiveWaitTime
 *		MediaLiveVideo
 *		MediaLiveAudio
 *-----------------------------------------------------------------------------------------
 */
pthread_mutex_t Stream::pubLicStreamPOSIXMutex = PTHREAD_MUTEX_INITIALIZER;

void Stream::pubLicStreamPOSIXMutexLOCK() {
#if 0
	while (pthread_mutex_trylock(&pubLicStreamPOSIXMutex) != 0);
#else
	pthread_mutex_lock(&pubLicStreamPOSIXMutex);
#endif
}

void Stream::pubLicStreamPOSIXMutexUNLOCK() {
	pthread_mutex_unlock(&pubLicStreamPOSIXMutex);
}

static void syncLiveWaitTime(std::shared_ptr<Stream> ref) {
	uint64_t nextTime;

	if (ref->mediaLive->getSampleTime_us(StreamSourceType::Video) > ref->mediaLive->getSampleTime_us(StreamSourceType::Audio)) {
		nextTime = ref->mediaLive->getSampleTime_us(StreamSourceType::Audio);
	}
	else {
		nextTime = ref->mediaLive->getSampleTime_us(StreamSourceType::Video);
	}

	auto currentTime = currentTimeInMicroSeconds();
	auto elapsed	 = currentTime - ref->mediaLive->startTime;

	if (nextTime > elapsed) {
		auto waitTime = nextTime - elapsed;

		Stream::pubLicStreamPOSIXMutexUNLOCK();
		usleep(waitTime);
		Stream::pubLicStreamPOSIXMutexLOCK();
	}
}

void Stream::MediaLiveVideo(bool isFullHD, uint8_t *samples, uint32_t totalSamples) {
	extern ClientsGroup_t clients;
	extern optional<shared_ptr<Stream>> avStream;

#if (CHECK_TIME_EXE == 1)
	std::cout << "[EXEC-TIME] MediaLiveVideo\n";
	auto start = std::chrono::high_resolution_clock::now();
#endif

	if (!avStream.has_value() || clients.empty()) {
		return;
	}

	auto avStreamValue	 = avStream.value();
	rtc::binary *pSample = nullptr;

	Stream::pubLicStreamPOSIXMutexLOCK();
	{
		syncLiveWaitTime(avStreamValue);
		avStreamValue->mediaLive->video->loadNextTime();
		avStreamValue->mediaLive->loadNextSample(samples, totalSamples, StreamSourceType::Video);
		pSample = &(avStreamValue->mediaLive->getSample(StreamSourceType::Video));
	}
	Stream::pubLicStreamPOSIXMutexUNLOCK();

	lockMutexListClients();
	{
		for (auto it : clients) {
			Client::lockSignalingClient();
			if (Client::getSignalingStatus()) {
				APP_DBG("ingore buffer video\n");
				Client::unLockSignalingClient();
				break;
			}
			Client::unLockSignalingClient();

			auto id			  = it.first;
			auto client		  = it.second;
			auto optTrackData = client->video;

			if (client->getState() == Client::State::Ready && optTrackData.has_value()) {
				if (client->getMediaStreamOptions() == Client::eOptions::LiveStream) {
					bool isValid = false;

					if (isFullHD) {
						isValid = (client->getLiveResolution() == LiveStream::eResolution::FullHD1080p) ? true : false;
					}
					else {
						isValid = (client->getLiveResolution() == LiveStream::eResolution::HD720p) ? true : false;
					}
					if (isValid == false) {
						continue;
					}

					auto trackData = optTrackData.value();
					auto rtpConfig = trackData->sender->rtpConfig;

					auto elapsedSeconds			= double(avStreamValue->mediaLive->getSampleTime_us(StreamSourceType::Video)) / (1000 * 1000);
					uint32_t elapsedTimestamp	= rtpConfig->secondsToTimestamp(elapsedSeconds);
					rtpConfig->timestamp		= rtpConfig->startTimestamp + elapsedTimestamp;
					auto reportElapsedTimestamp = rtpConfig->timestamp - trackData->sender->lastReportedTimestamp();
					if (rtpConfig->timestampToSeconds(reportElapsedTimestamp) > 1) {
						trackData->sender->setNeedsToReport();
					}

					try {
						trackData->track->send(*pSample);

#if 0
						std::cout << "Send to " << id << " ";
						std::cout << "samples video size " << pSample->size() << " ";
						std::cout << "with timestamp " << rtpConfig->timestamp << " ";
						std::cout << std::endl;
#endif
					}
					catch (const std::exception &e) {
						APP_DBG("[ERR] Unable to send video packet to client %s, err: %s\n", id.c_str(), e.what());
					}
				}
			}
		}
	}
	unlockMutexListClients();

#if (CHECK_TIME_EXE == 1)
	auto end										   = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << "[EXEC-TIME] MediaLiveVideo exe time: " << duration.count() << " milliseconds\n";
#endif

#if 0
	pSample->clear();
#else
	avStreamValue->mediaLive->rstSample(StreamSourceType::Video);
#endif
}

void Stream::MediaLiveAudio(uint8_t *samples, uint32_t totalSamples) {
	extern ClientsGroup_t clients;
	extern optional<shared_ptr<Stream>> avStream;

#if (CHECK_TIME_EXE == 1)
	std::cout << "[EXEC-TIME] MediaLiveAudio\n";
	auto start = std::chrono::high_resolution_clock::now();
#endif

	if (!avStream.has_value() || clients.empty()) {
		return;
	}

	auto avStreamValue	 = avStream.value();
	rtc::binary *pSample = nullptr;
	bool isEnableToSend	 = false;

	Stream::pubLicStreamPOSIXMutexLOCK();
	{
		syncLiveWaitTime(avStreamValue);
		avStreamValue->mediaLive->audio->loadNextTime();
		avStreamValue->mediaLive->loadNextSample(samples, totalSamples, StreamSourceType::Audio);
		pSample = &(avStreamValue->mediaLive->getSample(StreamSourceType::Audio));
		if (pSample->size() > (160 * 8)) { /* Accumulate audio sampels to make smooth sound */
			isEnableToSend = true;
		}
	}
	Stream::pubLicStreamPOSIXMutexUNLOCK();

	if (!isEnableToSend) {
		return;
	}

	lockMutexListClients();
	{
		for (auto it : clients) {
			Client::lockSignalingClient();
			if (Client::getSignalingStatus()) {
				APP_DBG("ingore buffer audio\n");
				Client::unLockSignalingClient();
				break;
			}
			Client::unLockSignalingClient();

			auto id			  = it.first;
			auto client		  = it.second;
			auto optTrackData = client->audio;

			if (client->getState() == Client::State::Ready && optTrackData.has_value()) {
				if (client->getMediaStreamOptions() == Client::eOptions::LiveStream) {
					auto trackData = optTrackData.value();
					auto rtpConfig = trackData->sender->rtpConfig;

					auto elapsedSeconds			= double(avStreamValue->mediaLive->getSampleTime_us(StreamSourceType::Audio)) / (1000 * 1000);
					uint32_t elapsedTimestamp	= rtpConfig->secondsToTimestamp(elapsedSeconds);
					rtpConfig->timestamp		= rtpConfig->startTimestamp + elapsedTimestamp;
					auto reportElapsedTimestamp = rtpConfig->timestamp - trackData->sender->lastReportedTimestamp();
					if (rtpConfig->timestampToSeconds(reportElapsedTimestamp) > 1) {
						trackData->sender->setNeedsToReport();
					}

					try {
						trackData->track->send(*pSample);

#if 0
						std::cout << "Send to " << id << " ";
						std::cout << "samples audio size " << pSample->size() << " ";
						std::cout << "with timestamp " << avStreamValue->mediaLive->getSampleTime_us(StreamSourceType::Audio);
						std::cout << std::endl;
#endif
					}
					catch (const std::exception &e) {
						APP_DBG("[ERR] Unable to send audio packet to client %s, err: %s\n", id.c_str(), e.what());
					}
				}
			}
		}
	}
	unlockMutexListClients();

#if (CHECK_TIME_EXE == 1)
	auto end										   = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << "[EXEC-TIME] MediaLiveAudio exe time: " << duration.count() << " milliseconds\n";
#endif

#if 0
	pSample->clear();
#else
	avStreamValue->mediaLive->rstSample(StreamSourceType::Audio);
#endif
}
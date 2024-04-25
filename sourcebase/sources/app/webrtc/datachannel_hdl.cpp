#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <mntent.h>
#include <unordered_map>

#include <fcntl.h>
#include <chrono>
#include <fstream>

#include "app.h"
#include "app_data.h"
#include "app_dbg.h"
// #include "firmware.h"
#include "task_list.h"

#include "rtc/rtc.hpp"
#include "datachannel_hdl.h"
#include "json.hpp"
// #include "SDCard.h"
// #include "helpers.hpp"
// #include "h26xsource.hpp"
#include "utils.h"
// #include "utilitiesd.hpp"
// #include "io_driver.h"

using namespace std;
using namespace rtc;
using json = nlohmann::json;
using namespace chrono;

#define PACKET_TRANSFER_SIZE (4096)	   // 5kb - 2096, 4096, 5120, 6144, 8192, 16384, 51200
static string qrId; /* Current client Id is queried */
static int disconnectHdl(json &content, bool &respFlag);
static int streamHdl(json &content, bool &respFlag);

static unordered_map<string, function<int(json &, bool &)>> dcCmdTblMaps = {
	// {DC_CMD_PANTILT,			 panTiltHdl   },
	//    {DC_CMD_PLAYLIST,			 playListHdl	},
	//    {DC_CMD_PLAYBACK,			 playBackHdl	},
	// {DC_CMD_DOWNLOAD,		  downloadHdl	 },
	   {DC_CMD_STREAM,			   streamHdl	},
	   {DC_CMD_DISCONNECT_REPORT, disconnectHdl},
	// {DC_CMD_PUSH_TO_TALK,	  pushToTalk	},
};

void onDataChannelHdl(const string clId, const string &req, string &resp) {
	json JSON;
	bool boolean = true;
	JSON		 = json::parse(req);

	/* If the ID is not correct -> do notthing */
	if (JSON["Id"].get<string>().compare(mtce_getSerialInfo()) != 0) {
		APP_DBG("The ID %s is not correct\n", JSON["Id"].get<string>().c_str());
		return;
	}

	/* Traverse to find valid command */
	auto it = dcCmdTblMaps.find(JSON["Command"].get<string>());
	boolean = (JSON["Type"].get<string>().compare("Request") == 0) ? true : false;
	if (it != dcCmdTblMaps.end() && boolean) {
		qrId	= clId;
		int ret = it->second(JSON.at("Content"), boolean);
		if (boolean) {
			JSON["Type"]			  = "Respond";
			JSON["Result"]["Ret"]	  = ret;
			JSON["Result"]["Message"] = (ret == APP_CONFIG_SUCCESS ? "Success" : "Fail");
			resp.assign(JSON.dump());
		}
	}
	else {
		APP_DBG("Invalid command %s\n", JSON["Command"].get<string>().c_str());
	}
}

int disconnectHdl(json &content, bool &respFlag) {
	(void)(respFlag);
	APP_DBG("[MANUAL] clear client id: %s\n", qrId.c_str());
	task_post_dynamic_msg(GW_TASK_WEBRTC_ID, GW_WEBRTC_ERASE_CLIENT_REQ, (uint8_t *)qrId.c_str(), qrId.length() + 1);
	return APP_CONFIG_SUCCESS;
}

int streamHdl(json &content, bool &respFlag) {

	APP_DBG("HELLO WORLKD\n");
// 	(void)(respFlag);
// 	int rc = APP_CONFIG_SUCCESS;
// 	APP_DBG("streamHdl() -> %s\n", content.dump(4).c_str());
// #if (CHECK_TIME_EXE == 1)
// 	std::cout << "[EXEC-TIME] streamHdl\n";
// 	auto start = std::chrono::high_resolution_clock::now();
// #endif

// 	Client::eOptions opt		= content["Option"].get<Client::eOptions>();
// 	LiveStream::eResolution res = content["Resolution"].get<LiveStream::eResolution>();

// 	if ((opt != Client::eOptions::Idle && opt != Client::eOptions::LiveStream && opt == Client::eOptions::Playback) ||
// 		(res != LiveStream::eResolution::HD720p && res != LiveStream::eResolution::FullHD1080p)) {
// 		return APP_CONFIG_ERROR_DATA_INVALID;
// 	}

// 	auto it = clients.find(qrId);
// 	if (it == clients.end()) {
// 		return APP_CONFIG_ERROR_ANOTHER;
// 	}
// 	auto qrClient = it->second;

// 	/* Send previous NALU key frame so users don't have to wait to see stream works */
// 	bool isRstTimestamp = false;
// 	if (qrClient->getMediaStreamOptions() != Client::eOptions::LiveStream) {
// 		isRstTimestamp = true;
// 	}
// 	if (opt == Client::eOptions::LiveStream) {
// 		// if (opt == Client::eOptions::LiveStream && qrClient->getMediaStreamOptions() != Client::eOptions::LiveStream) {
// 		Stream::pubLicStreamPOSIXMutexLOCK();

// 		auto avStreamValue = avStream.value();
// 		auto h264		   = dynamic_cast<H26XSource *>(avStreamValue->mediaLive->video.get());
// 		auto nalUnits	   = (res == LiveStream::eResolution::FullHD1080p) ? (Stream::nalUnitsSub) : (Stream::nalUnitsMain);

// 		Stream::pubLicStreamPOSIXMutexUNLOCK();

// 		if (nalUnits.size()) {
// 			std::cout << "Send Initial NAL Units size " << nalUnits.size() << std::endl;

// 			lockMutexListClients();

// 			auto video = qrClient->video.value();
// 			if (isRstTimestamp) {
// 				const double frameDuration_s		  = double(h264->getSampleDuration_us()) / (1000 * 1000);
// 				const uint32_t frameTimestampDuration = video->sender->rtpConfig->secondsToTimestamp(frameDuration_s);
// 				video->sender->rtpConfig->timestamp	  = video->sender->rtpConfig->startTimestamp - frameTimestampDuration * 2;
// 			}

// 			try {
// 				video->track->send(nalUnits);
// 			}
// 			catch (...) {
// 			}

// 			unlockMutexListClients();
// 		}
// 		else {
// 			std::cout << "Initial NAL Units not found" << std::endl;
// 		}
// 	}

// 	qrClient->setMediaStreamOptions(opt);
// 	qrClient->setLiveResolution(res);

// 	content.clear();

// #if (CHECK_TIME_EXE == 1)
// 	auto end										   = std::chrono::high_resolution_clock::now();
// 	std::chrono::duration<double, std::milli> duration = end - start;
// 	std::cout << "[EXEC-TIME] streamHdl exe time: " << duration.count() << " milliseconds\n";
// #endif

// 	return rc;
}
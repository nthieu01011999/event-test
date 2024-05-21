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
#include "helpers.hpp"
#include "h26xsource.hpp"
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
    (void)(respFlag);
    int rc = APP_CONFIG_SUCCESS;
    APP_DBG("streamHdl() -> %s\n", content.dump(4).c_str());

#if (CHECK_TIME_EXE == 1)
    std::cout << "[EXEC-TIME] streamHdl\n";
    auto start = std::chrono::high_resolution_clock::now();
#endif

    // Parse options and resolution from content
    Client::eOptions opt = content["Option"].get<Client::eOptions>();
    LiveStream::eResolution res = content["Resolution"].get<LiveStream::eResolution>();

    APP_DBG("Parsed options: Option=%d, Resolution=%d\n", opt, res);

    // Validate options and resolution
    if ((opt != Client::eOptions::Idle && opt != Client::eOptions::LiveStream && opt != Client::eOptions::Playback) ||
        (res != LiveStream::eResolution::HD720p && res != LiveStream::eResolution::FullHD1080p)) {
        APP_DBG("Invalid option or resolution\n");
        return APP_CONFIG_ERROR_DATA_INVALID;
    }

    // Find the client
    auto it = clients.find(qrId);
    if (it == clients.end()) {
        APP_DBG("Client not found\n");
        return APP_CONFIG_ERROR_ANOTHER;
    }
    auto qrClient = it->second;
    APP_DBG("Client found\n");

    // Check and set timestamp reset flag
    bool isRstTimestamp = qrClient->getMediaStreamOptions() != Client::eOptions::LiveStream;

    // Handle LiveStream option
    if (opt == Client::eOptions::LiveStream) {
        Stream::pubLicStreamPOSIXMutexLOCK();

        if (!avStream.has_value()) {
            Stream::pubLicStreamPOSIXMutexUNLOCK();
            APP_DBG("avStream does not have a value\n");
            return APP_CONFIG_ERROR_ANOTHER;
        }

        auto avStreamValue = avStream.value();

        // Obtain a shared pointer to the video source
        APP_DBG("Attempting to get video source and cast to H26XSource\n");
        auto videoSource = avStreamValue->mediaLive->video;

        // Attempt to cast the video source to an H26XSource pointer
        H26XSource *h264 = dynamic_cast<H26XSource *>(videoSource.get());

        // Check if the cast was successful
        if (!h264) {
            Stream::pubLicStreamPOSIXMutexUNLOCK();
            APP_DBG("Failed to cast video source to H26XSource\n");
            return APP_CONFIG_ERROR_ANOTHER;
        }

        APP_DBG("Successfully cast video source to H26XSource\n");

        // Determine the appropriate NAL units based on the resolution
        auto nalUnits = (res == LiveStream::eResolution::FullHD1080p) ? Stream::nalUnitsSub : Stream::nalUnitsMain;

        Stream::pubLicStreamPOSIXMutexUNLOCK();
        APP_DBG("NAL Units size: %d\n", nalUnits.size());

        if (!nalUnits.empty()) {
            lockMutexListClients();

            if (!qrClient->video.has_value()) {
                APP_DBG("qrClient->video does not have a value\n");
                unlockMutexListClients();
                return APP_CONFIG_ERROR_ANOTHER;
            }

            auto video = qrClient->video.value();

            // Reset timestamp if needed
            if (isRstTimestamp) {
                const double frameDuration_s = static_cast<double>(h264->getSampleDuration_us()) / (1000 * 1000);
                const uint32_t frameTimestampDuration = video->sender->rtpConfig->secondsToTimestamp(frameDuration_s);
                video->sender->rtpConfig->timestamp = video->sender->rtpConfig->startTimestamp - frameTimestampDuration * 2;
                APP_DBG("Timestamp reset for video stream\n");
            }

            // Send NAL Units
            try {
                video->track->send(nalUnits);
                APP_DBG("Sent NAL Units successfully\n");
            } catch (const std::exception &e) {
                APP_DBG("Exception while sending NAL Units: %s\n", e.what());
            }

            unlockMutexListClients();
        } else {
            APP_DBG("No NAL Units found\n");
        }
    }

    // Update client media stream options and resolution
    qrClient->setMediaStreamOptions(opt);
    qrClient->setLiveResolution(res);

    content.clear();

#if (CHECK_TIME_EXE == 1)
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "[EXEC-TIME] streamHdl exe time: " << duration.count() << " milliseconds\n";
#endif

    APP_DBG("Returning success [STREAM]\n");
    return rc;
}

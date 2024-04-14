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
// #include "h26xsource.hpp"
#include "utils.h"
// #include "utilitiesd.hpp"
// #include "io_driver.h"

using namespace std;
using namespace rtc;
using json = nlohmann::json;
using namespace chrono;

// #define PACKET_TRANSFER_SIZE (16384)	//16kb
#define PACKET_TRANSFER_SIZE (51200)	// 50kb

static int panTiltHdl(json &content, bool &respFlag);
static int playListHdl(json &content, bool &respFlag);
static int playBackHdl(json &content, bool &respFlag);
static int downloadHdl(json &content, bool &respFlag);
static int streamHdl(json &content, bool &respFlag);
static int disconnectHdl(json &content, bool &respFlag);
static int pushToTalk(json &content, bool &respFlag);

static unordered_map<string, function<int(json &, bool &)>> dcCmdTblMaps = {
	// {DC_CMD_PANTILT,			 panTiltHdl   },
	// {DC_CMD_PLAYLIST,		  playListHdl	 },
	// {DC_CMD_PLAYBACK,		  playBackHdl	 },
	// {DC_CMD_DOWNLOAD,		  downloadHdl	 },
	// {DC_CMD_STREAM,			streamHdl	 },
	// {DC_CMD_DISCONNECT_REPORT, disconnectHdl},
	// {DC_CMD_PUSH_TO_TALK, pushToTalk},
};

static string qrId; /* Current client Id is queried */

 

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

int panTiltHdl(json &content, bool &respFlag) {
	// (void)(respFlag);
	// int rc = APP_CONFIG_SUCCESS;
	// APP_DBG("panTiltHdl() -> %s\n", content.dump(4).c_str());

	// if (content["Direction"].get<string>().compare("Up") == 0) {
	// 	driveIO.stepMotors.makeStep(StepMotors::TILT::UP);
	// }
	// else if (content["Direction"].get<string>().compare("Down") == 0) {
	// 	driveIO.stepMotors.makeStep(StepMotors::TILT::DOWN);
	// }
	// else if (content["Direction"].get<string>().compare("Left") == 0) {
	// 	driveIO.stepMotors.makeStep(StepMotors::PAN::LEFT);
	// }
	// else if (content["Direction"].get<string>().compare("Right") == 0) {
	// 	driveIO.stepMotors.makeStep(StepMotors::PAN::RIGHT);
	// }
	// else {
	// 	rc = APP_CONFIG_ERROR_DATA_INVALID;
	// }
	// content.clear();

	// return rc;
}
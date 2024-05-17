#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <chrono>

#include "ak.h"
#include "timer.h"
#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "utils.h"
#include "stream.hpp"

#include "app_config.h"
#include "mtce_video.hpp"
#include "parser_json.h"
#include "task_list.h"
#include "h26xsource.hpp"
#include "task_av.h" 

#define VIDEO_DEBUG 1
#define TEST_SET_CONFIG			 0
#define TOTAL_PROCESS_STOP		 4
#define CAPTURE_CHECK_COUTER_MAX (30)	 // 5min
#define CAPTURE_PROCESS_NAME	 "capture"
using namespace chrono;

q_msg_t gw_task_av_mailbox;

static pthread_mutex_t mtxStreamVideo = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mtxListClients = PTHREAD_MUTEX_INITIALIZER;

// VideoCtrl videoCtrl;
static int8_t captureRetryCounter = 0;
static int8_t streamRetryCounter  = 0;



VideoCtrl videoCtrl;




void *gw_task_av_entry(void *) {
    wait_all_tasks_started();
    APP_DBG("[STARTED] Task AV\n");
    ak_msg_t *msg = AK_MSG_NULL;

    task_post_pure_msg(GW_TASK_AV_ID, GW_AV_INIT_REQ);
    // task_post_pure_msg(GW_TASK_AV_ID, GW_AV_CHECK_CAPTURE_RUNNING_REQ);
    // timer_set(GW_TASK_AV_ID, GW_AV_CHECK_VIDEO_STREAM_RUNNING_REQ, 60000, TIMER_ONE_SHOT);

    while (1) {
        msg = ak_msg_rev(GW_TASK_AV_ID);

        switch (msg->header->sig) {
        case GW_AV_INIT_REQ: {
            APP_DBG_SIG("GW_AV_INIT_REQ\n");
			mtce_encode_t encode = {0};
            if (videoCtrl.loadConfigFromFileChannels(&encode) == APP_CONFIG_SUCCESS) {
            int ret = videoCtrl.setVideoEncodeChannels(&encode);
            APP_DBG("[loadConfigFromFileChannels]\n");
            }
        } break;

	// 	case GW_AV_CHECK_CAPTURE_RUNNING_REQ: {
	// 		APP_DBG_SIG("GW_AV_CHECK_CAPTURE_RUNNING_REQ\n");

	// 		string cmd = "pidof -s " CAPTURE_PROCESS_NAME " > /dev/null 2>&1";
	// 		if (systemCmd(cmd.c_str()) == 0) {	  // ok
	// 			APP_PRINT("capture process is running\n");
	// 			captureRetryCounter = 0;
	// 		}
	// 		else if (++captureRetryCounter <= CAPTURE_CHECK_COUTER_MAX) {
	// 			APP_PRINT("captureRetryCounter retry: %d\n", captureRetryCounter);
	// 		}
	// 		else {
	// 			task_post_pure_msg(GW_TASK_SYS_ID, GW_SYS_REBOOT_REQ);
	// 		}
	// 	} break;

    //     case GW_AV_CHECK_VIDEO_STREAM_RUNNING_REQ: {
    //         APP_DBG_SIG("GW_AV_CHECK_VIDEO_STREAM_RUNNING_REQ\n");
    //         if (!videoCtrl.getStreamIsRunningChannel(MTCE_MAIN_STREAM) || !videoCtrl.getStreamIsRunningChannel(MTCE_SUB_STREAM)) {
    //             APP_PRINT("video not streaming, start reboot camera\n");
    //             task_post_pure_msg(GW_TASK_SYS_ID, GW_SYS_REBOOT_REQ);
    //         } else {
    //             APP_PRINT("video is streaming\n");
    //             timer_set(GW_TASK_AV_ID, GW_AV_CHECK_VIDEO_STREAM_RUNNING_REQ, 60000, TIMER_ONE_SHOT);
    //         }
    //     } break;

        default:
            break;
        }

    //     // Free the message
        ak_msg_free(msg);
    }

    return (void *)0;
}

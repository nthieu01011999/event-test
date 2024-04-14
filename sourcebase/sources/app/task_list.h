#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "ak.h"
#include "message.h"

#include "app_data.h"

// #include "if_console.h"

// #include "task_console.h"
// #include "task_cloud.h"
// #include "task_sys.h"
// #include "task_config.h"
// #ifdef BUILD_ARM_VVTK
// #include "task_av.h"
// #endif
// #include "task_detect.h"
// #include "task_record.h"
// #include "task_upload.h"
// #include "task_network.h"
#include "task_webrtc.h"
// #include "task_fw.h"
// #ifdef FEATURE_RTMP
// #include "task_rtmp.h"
// #endif

/** default if_des_type when get pool memory
 * this define MUST BE coresponding with app.
 */
#define AK_APP_TYPE_IF 101

enum {
	/* SYSTEM TASKS */
	AK_TASK_TIMER_ID,

	/* APP TASKS */
	GW_TASK_IF_CONSOLE_ID,
	GW_TASK_CONSOLE_ID,
	GW_TASK_CLOUD_ID,
	GW_TASK_SYS_ID,
#ifdef BUILD_ARM_VVTK
	GW_TASK_AV_ID,
	GW_TASK_DETECT_ID,
#endif
	GW_TASK_CONFIG_ID,
	GW_TASK_RECORD_ID,
	GW_TASK_UPLOAD_ID,
	GW_TASK_NETWORK_ID,
	GW_TASK_FW_ID,
	GW_TASK_WEBRTC_ID,
#ifdef FEATURE_RTMP
	GW_TASK_RTMP_ID,
#endif

	/* EOT task ID */
	AK_TASK_LIST_LEN,
};

extern ak_task_t task_list[];

#endif	  //__TASK_LIST_H__

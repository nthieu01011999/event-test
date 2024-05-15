#include "timer.h"

#include "task_list.h"

ak_task_t task_list[] = {
	/* SYSTEM TASKS */
	{	AK_TASK_TIMER_ID,	   	TASK_PRI_LEVEL_1, 	timer_entry,			  	&timer_mailbox,			  		"timer service"	 	},
	{	GW_TASK_WEBRTC_ID,		TASK_PRI_LEVEL_1, 	gw_task_webrtc_entry,		&gw_task_webrtc_mailbox,	 	"webrtc"		 	},
	{	GW_TASK_HELLO_ID,		TASK_PRI_LEVEL_1, 	gw_task_hello_entry,		&gw_task_hello_mailbox,			"hello_world"		},
	{	GW_TASK_AV_ID,			TASK_PRI_LEVEL_1, 	gw_task_av_entry,			&gw_task_av_mailbox,			"AV_init"		},

};

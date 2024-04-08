#define DEBUG

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <vector>

#include <algorithm>
#include <future>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <atomic>

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"



#ifdef DEBUG
#define LOG(msg) std::cout << (msg) << std::endl
#else
#define LOG(msg)
#endif

// Usage

using namespace std;

q_msg_t gw_task_webrtc_mailbox;

void *gw_task_webrtc_entry(void *) {
	ak_msg_t *msg = AK_MSG_NULL;

	wait_all_tasks_started();

    #ifdef DEBUG
    std::cout << "Debug mode: ON" << std::endl;
    // Debug-specific code here
    #else
    std::cout << "Debug mode: OFF" << std::endl;
    // Production code here
    #endif

	APP_DBG("[STARTED] gw_task_webrtc_entry\n");

	while (1) {
		/* get messge */
		msg = ak_msg_rev(GW_TASK_WEBRTC_ID);

		switch (msg->header->sig) {
			
		default:
		break;
		}

		/* free message */
		ak_msg_free(msg);
	}

	return (void *)0;
}



#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CUnit/Basic.h>
#include "vvtk_event.h"

static int success = 0;

void test_event_roi_config()
{
	vvtk_roi_t roi;
	
	roi.roi_grid[0] = 0x00;
	roi.roi_grid[1] = 0x00;
	roi.roi_grid[2] = 0x00;
	roi.roi_grid[3] = 0x00;
	roi.roi_grid[4] = 0xFF;
	roi.roi_grid[5] = 0xFF;
	roi.roi_grid[6] = 0xFF;
	roi.roi_grid[7] = 0xFF;
	
	roi.sensitivity = 1;		
	CU_ASSERT(vvtk_set_roi_grid(&roi) == 0);
	memset(&roi, 0, sizeof(roi));
	CU_ASSERT(vvtk_get_roi_grid(&roi) == 0);
	CU_ASSERT(roi.roi_grid[0] == 0x00 && roi.roi_grid[1] == 0x00 &&
			roi.roi_grid[2] == 0x00 && roi.roi_grid[3] == 0x00 && 
			roi.roi_grid[4] == 0xFF && roi.roi_grid[5] == 0xFF && 
			roi.roi_grid[6] == 0xFF && roi.roi_grid[7] == 0xFF);
	
	CU_ASSERT(roi.sensitivity == 1);
}

void test_event_md_config()
{
	vvtk_motion_config_t config;

	config.interval = 500;

	CU_ASSERT(vvtk_set_motion_config(&config) == 0);
	
	config.interval = 0;
	CU_ASSERT(vvtk_get_motion_config(&config) == 0);

	CU_ASSERT(config.interval == 500);
}

void test_event_get_capbility()
{
	vvtk_roi_capability_t cap;
	CU_ASSERT(vvtk_get_roi_capability(&cap) == 0);
	CU_ASSERT(cap.roi_num == 64 && cap.type.polygon == 0 && cap.type.square == 0 && cap.type.grid == 1);
}

void callback_func(vvtk_motion_event_t* event_info)
{
	int i;
	
	if(event_info->motion_info_size > 0) {
		for(i=0; i<event_info->motion_info_size; i++){
			printf("roi_indx = %d\n", event_info->motion_info_list[i].roi_index);
		}
	}
	if(event_info->timestamp >0)
		success = 1;
}
void test_event_run_md()
{
	success = 0;
	int count = 0;
	CU_ASSERT(vvtk_set_motion_event_callback(callback_func) == 0);
	
	printf("\nTry to make an event trigger in 120 seconds....\n");
	
	while(!success && count<60) {
	//while(count<120) {
		sleep(1);
		count++;
	};
	
	CU_ASSERT(success == 1);
}

void test_event_stop_md()
{
	CU_ASSERT(vvtk_stop_motion_detection() == 0);
	sleep(5);
}

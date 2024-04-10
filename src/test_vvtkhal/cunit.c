#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CUnit/Basic.h>
#include "cunit.h"
#include "test_vvtkhal.h"
#include "test_io_api.h"
#include "test_audio_api.h"
#include "test_wifi_api.h"
#include "test_video_api.h"
#include "test_camera_api.h"
#include "test_event_api.h"
#include "test_system_api.h"

static CU_TestInfo tests_audio[] = {
    {"Audio output volume", test_vvtk_audio_output_volume},
    {"Audio config", test_vvtk_audio_config},
    {"Audio callback", test_vvtk_audio_callback},
    {"Audio output", test_vvtk_audio_output},
    {"Audio file playback", test_vvtk_audio_file_playback},
    {"Audio g711 audio output", test_vvtk_audio_g711_output},
    {"Audio input volume", test_vvtk_audio_input_volume},
    CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_video[] = {
    {"Video callback", test_vvtk_video_callback},
    {"Video config", test_vvtk_video_config},
    {"Video jpeg image", test_vvtk_video_jpeg_image},
    CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_io[] = {
    {"LED set and get", test_vvtk_set_get_led_status},
    {"Button test", test_vvtk_set_get_button},
    {"Motor test", test_vvtk_motor},
    {"SDcard test", test_sdcard},
    {"Lighting White LED", test_vvtk_led_lighting},
    {"Light Sensor", test_vvtk_get_light_sensor},
    CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_wifi[] = {
	{"Initial wifi", test_wifi_init},
	{"wifi scan ap", test_wifi_scan},
	{"wifi connect", test_wifi_connect},
	{"wifi ap", test_wifi_ap},
    CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_camera[] = {
    {"ISP config", test_vvtk_isp_config},
    {"Day/Night mode", test_vvtk_daynight_mode},
    {"Anti-flicker", test_vvtk_anti_flicker},
    {"Privacy mask", test_vvtk_privacy_mask},
    {"OSD time pos", test_vvtk_osd_time_pos},
    {"WB config", test_vvtk_wb_config},
    {"WDR config", test_vvtk_wdr_config},
    {"DNR config", test_vvtk_dnr_config},
    {"Day/Night mode ex", test_vvtk_daynight_mode_ex},
    CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_event[] = {
    {"Event roi config", test_event_roi_config},
    {"Event md config", test_event_md_config},
    {"Event md capability", test_event_get_capbility},
    {"Event run md", test_event_run_md},
    //{"Event stop md", test_event_stop_md},
    CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_system[] = {
    //{"System access SN", test_system_access_sn},
    //{"System get mac", test_system_get_mac},
   // {"System HW Info", test_system_get_hw_info},
  //  {"System enable network", test_system_start_network},
    {"System get_capability",test_system_get_capability},
    CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_fwupgrade[] = {
    {"FW Upgrade", test_system_fw_upgrade},
    CU_TEST_INFO_NULL,
};

static CU_SuiteInfo test_suite_video = {"Test Video API", NULL, NULL, NULL, NULL, tests_video};
static CU_SuiteInfo test_suite_audio = {"Test Audio API", NULL, NULL, NULL, NULL, tests_audio};
static CU_SuiteInfo test_suite_wifi = {"Test Wifi API", NULL, NULL, NULL, NULL, tests_wifi};
static CU_SuiteInfo test_suite_io = {"Test IO API", NULL, NULL, NULL, NULL, tests_io};
static CU_SuiteInfo test_suite_camera = {"Test Camera API", NULL, NULL, NULL, NULL, tests_camera};
static CU_SuiteInfo test_suite_event = {"Test Event API", NULL, NULL, NULL, NULL, tests_event};
static CU_SuiteInfo test_suite_system = {"Test System API", NULL, NULL, NULL, NULL, tests_system};
static CU_SuiteInfo test_suite_fwupgrade = {"Test FW Upgrade API", NULL, NULL, NULL, NULL, tests_fwupgrade};

static CU_SuiteInfo test_suite_null = CU_SUITE_INFO_NULL;

static CU_SuiteInfo *suites = NULL;
static int suites_size = 0;

void add_test_suite(CU_SuiteInfo test_suite)
{
  suites_size++;
  suites = (struct CU_SuiteInfo *)realloc(suites, suites_size * sizeof(struct CU_SuiteInfo));
  suites[suites_size - 1] = test_suite;
}

void add_test_suite_index(int index)
{
  CU_SuiteInfo *new_test_case;
  switch (index)
  {

  case TEST_VIDEO_API:
    add_test_suite(test_suite_video);
    break;
  case TEST_AUDIO_API:
    add_test_suite(test_suite_audio);
    break;
  case TEST_IO_API:
    add_test_suite(test_suite_io);
    break;
  case TEST_WIFI_API:
    add_test_suite(test_suite_wifi);
    break;
  case TEST_CAMERA_API:
    add_test_suite(test_suite_camera);
    break;
  case TEST_EVENT_API:
    add_test_suite(test_suite_event);
    break;
  case TEST_SYSTEM_API:
    add_test_suite(test_suite_system);
    break;
  case TEST_FWUPGRADE_API:
    add_test_suite(test_suite_fwupgrade);
    break;
  case -1:
    add_test_suite(test_suite_null);
    break;
  default:
    printf("Invalid option: %d\n", index);
    return 1;
  }
  return 0;
}

void add_cunit_test()
{
  assert(NULL != CU_get_registry());
  assert(!CU_is_test_running());
  /* Register suites. */
  if (CU_register_suites(suites) != CUE_SUCCESS)
  {
    fprintf(stderr, "suite registration failed - %s\n",
            CU_get_error_msg());
    exit(EXIT_FAILURE);
  }
}

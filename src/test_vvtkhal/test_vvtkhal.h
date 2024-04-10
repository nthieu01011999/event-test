#ifndef __TEST_VVTKHAL_H__
#define __TEST_VVTKHAL_H__
/*
Ref:https://cunit.sourceforge.net/doc/writing_tests.html
*/
enum{
    TEST_VIDEO_API =0,
    TEST_AUDIO_API ,
    TEST_IO_API,
    TEST_WIFI_API,
    TEST_CAMERA_API,
    TEST_EVENT_API,
    TEST_SYSTEM_API,
    TEST_FWUPGRADE_API,
};

typedef struct{
	int index;
    char *testcase;
}test_case_desc_t;




#endif /*__TEST_VVTKHAL_H__*/

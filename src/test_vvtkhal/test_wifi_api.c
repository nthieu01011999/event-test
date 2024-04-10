#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CUnit/Basic.h>
#include "vvtk_wifi.h"

void scan_result_show(wifi_network_info* aps, int number)
{
	int i;
	
	printf("Show result: count = %d\n", number);

	for(i=0; i<number; i++) {
		printf("index: %d \tssid: %s\t signal:%d\t, encrypt:%s \t\n",
				i, (aps+i)->ssid, (aps+i)->signal_strength, (aps+i)->encrypt_mode);
		
	}
	
}

void test_wifi_init(void)
{
	CU_ASSERT(vvtk_wifi_init(0)==0);
}

void test_wifi_scan(void)
{
	CU_ASSERT(0 == vvtk_wifi_start_scan(scan_result_show));
}

void test_wifi_connect(void)
{
	//utf-8

	CU_ASSERT(0 == vvtk_wifi_connect("74657374", "12345678"));
	CU_ASSERT(0 != vvtk_wifi_connect("7465737434", "12345678"));
}

int check_hostapd()
{
	char buf[64 + 1] = {0};
	FILE* fp = 0;
	int ret = 1;

	fp =popen("ps|grep \"hostapd\"", "r");
	if(!fp) {
		return -1;
	}

	while(fgets(buf, 64, fp) != NULL) {
		if(strstr(buf, "\/usr\/bin\/hostapd")){
			ret = 0;
			break;
		}
	}

	fclose(fp);

	return ret;
}

void test_wifi_ap(void)
{
	CU_ASSERT(vvtk_wifi_init(1)==0);

	CU_ASSERT(0 == vvtk_wifi_configure_ap_mode("\xe6\xb8\xac\xe8\xa9\xa6\x53\x53\x49\x44", "12345678", 1, 1, "192.168.66.1"));

	CU_ASSERT(0 == vvtk_wifi_toggle_ap_mode(1));
	sleep(2);
	CU_ASSERT(0 == check_hostapd());

	sleep(5);
	printf("Turn off AP\n");
	CU_ASSERT(0 == vvtk_wifi_toggle_ap_mode(0));
	sleep(2);
	CU_ASSERT(1 == check_hostapd());
}


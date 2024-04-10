#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CUnit/Basic.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include "vvtk_io.h"

#define RST_BTN_DEV "/sys/class/gpio/gpio44/value"
static int test_btn_callback = 0;
static int test_sdcard_callback = 0;
char *g_user_data;
fd_set fdReadSet;
int sock = STDIN_FILENO;

vvtk_motor_status_t g_pan_status={0}, g_tilt_status={0};

VVTK_MOTOR_CALLBACK zero_cb(vvtk_motor_status_t status, const void *user_data){
    g_user_data = user_data;
}

VVTK_MOTOR_CALLBACK pan_cb(vvtk_motor_status_t status, const void *user_data)
{
	g_user_data = user_data;
	printf("\n [pan]g_user_data = %s \n",g_user_data);
	g_pan_status.step = status.step;
	g_pan_status.status = status.status;
	printf("\n step = %d, status.status=%d \n",status.step, status.status);
}

VVTK_MOTOR_CALLBACK tilt_cb(vvtk_motor_status_t status, const void *user_data)
{
	g_user_data = user_data;
	printf("\n [tilt]g_user_data = %s \n",g_user_data);
	g_tilt_status.step = status.step;
	g_tilt_status.status = status.status;
	printf("\n step = %d, status.status=%d \n",status.step, status.status);
}


VVTK_BTN_CALLBACK io_btn_callback(unsigned long long t, const VVTK_BTN_STATUS status)
{
	test_btn_callback = 1;
	return 0;
}

VVTK_SD_CALLBACK sdcard_change_callback(VVTK_SD_STATUS status, const void *user_data){
	g_user_data = user_data;
	printf("\n [sdcard]g_user_data = %s, status= %d \n",g_user_data, status);
    test_sdcard_callback=1;
}

// if == 0, then there is data to be read on stdin
int detect_stdin()
{
	//select socket
	struct timeval timeout;
	timeout.tv_sec = 20;
	timeout.tv_usec = 0;

	FD_ZERO(&fdReadSet);
	FD_SET(sock,&fdReadSet);

    printf("\r\n>>>press enter to continue<<<\n");
	int iSelectRet = select(sock+1,&fdReadSet,NULL,NULL,&timeout);
	if(iSelectRet > 0){
		if(FD_ISSET(sock,&fdReadSet)) //test socket readable or not
		{
			char szMediaBuffer[64];
			unsigned int uMediaBufferLen = 64;
			memset(szMediaBuffer, 0, uMediaBufferLen);
			ssize_t ssizeRecvBytes = read(sock, szMediaBuffer, uMediaBufferLen);
			//printf("\r\n>>> rec %d bytes\n",ssizeRecvBytes);
			if(ssizeRecvBytes <= 0)
			{
				printf("\r\n>>>socket read error\n");
			}
		}
	}
	else if(iSelectRet==0){
		printf("\r\n>>> timeout\n");
		return -1;
	}
	else {
		perror("select");
        return -2;
	}
    return 0;
}

int led_on(int coloe){

}

int led_setStatus(vvtk_led_status_t led)
{
    int err;
    //printf("\nled.color=%d",led.color);
    //printf("\nled.mode=%d",led.mode);
    //printf("\nled.interval=%d",led.interval);
    vvtk_led_status_t tmp;
    err = vvtk_set_led_status(led);

    if (err)
    {
        printf("[led] set status error %d ------------------------------- \n", err);
        return err;
    }
    tmp.color=led.color;
    err = vvtk_get_led_status(&tmp);

    if (err)
    {
        printf("[led] get status error %d ------------------------------- \n", err);
        return err;
    }

    if (memcmp(&led, &tmp, sizeof(tmp)) != 0)
    {
        printf("[led] set status error diff set and get value ------------------------------- \n");
        return -1;
    }

    printf("[led] set status success \n");
    return 0;
}

void test_vvtk_set_get_led_status(void)
{
    vvtk_led_status_t led;

    led.color = VVTK_LED_COLOR_GREEN;
    led.mode = VVTK_LED_MODE_BLINKING;
    led.interval = 500;
    printf("\n[led] test green blink 0.5s ++++ \n");
    led_setStatus(led);
    sleep(5);

    led.color = VVTK_LED_COLOR_GREEN;
    led.mode = VVTK_LED_MODE_OFF;
    printf("[led] test green off ++++ \n");
    led_setStatus(led);
    sleep(3);

    led.color = VVTK_LED_COLOR_GREEN;
    led.mode = VVTK_LED_MODE_ON;
    printf("[led] test green on ++++ \n");
    led_setStatus(led);
    sleep(3);

    led.color = VVTK_LED_COLOR_RED;
    led.mode = VVTK_LED_MODE_BLINKING;
    led.interval = 1000;
    printf("[led] test red blink 1s ++++ \n");
    led_setStatus(led);
    sleep(6);

    led.color = VVTK_LED_COLOR_RED;
    led.mode = VVTK_LED_MODE_ON;
    printf("[led] test red on ++++ \n");
    led_setStatus(led);
    sleep(3);

    led.color = VVTK_LED_COLOR_RED;
    led.mode = VVTK_LED_MODE_BLINKING;
    led.interval = 500;
    printf("[led] test red blink 0.5s ++++ \n");
    led_setStatus(led);
    sleep(6);

    led.color = VVTK_LED_COLOR_RED;
    led.mode = VVTK_LED_MODE_OFF;
    printf("[led] test red off ++++ \n");
    led_setStatus(led);
    sleep(3);

    led.color = VVTK_LED_COLOR_GREEN;
    led.mode = VVTK_LED_MODE_BLINKING;
    led.interval = 1000;
    printf("[led] test green blink 1s ++++ \n");
    led_setStatus(led);
    sleep(6);

    led.color = VVTK_LED_COLOR_GREEN;
    led.mode = VVTK_LED_MODE_BLINKING;
    led.interval = 200;
    printf("[led] test green blink 0.2s ++++ \n");
    led_setStatus(led);
    sleep(6);

    led.color = VVTK_LED_COLOR_GREEN;
    led.mode = VVTK_LED_MODE_OFF;
    printf("[led] test green off ++++ \n");
    led_setStatus(led);
    sleep(3);
}

void test_vvtk_set_get_button(void){

	int ret = 0;
	VVTK_BTN_STATUS status=VVTK_BTN_STATUS_UNKNOWN;
	struct timeval prev_tv;
	struct timeval tv;

	ret = vvtk_set_button_status_callback(VVTK_BTN_TYPE_RESET, io_btn_callback);
	CU_ASSERT(ret == 0);

	ret = vvtk_get_button_status(VVTK_BTN_TYPE_RESET, &status);
    if(status == VVTK_BTN_STATUS_PRESS){
        printf("\r\n Button pressed\r\n");
    }
    else if(status == VVTK_BTN_STATUS_RELEASE){
        printf("\r\n Button released\r\n");
    }
    CU_ASSERT(status > 0);

    printf("\r\n[%s]please push and hold the reset button 2s.\n",__FUNCTION__);
    ret = detect_stdin();
    CU_ASSERT(ret == 0);
    if(ret < 0){
        printf("\r\n[%s] timeout.\n",__FUNCTION__);
        return 1;
    }
    CU_ASSERT(test_btn_callback > 0);

}


void test_vvtk_motor(void){
    vvtk_motor_action_t action;
    int i=0, max_steps=0;
    char user_data_pan[128]="Pan Test\n";
    char user_data_tilt[128]="Tilt Test\n";
    char user_data_zero[128]="Zero\n";
    vvtk_get_motor_max_steps(VVTK_MOTOR_PAN,&max_steps);
    printf("\r\n Pan max_steps:%d",max_steps);
    vvtk_get_motor_max_steps(VVTK_MOTOR_TILT,&max_steps);
    printf("\r\n Tilt max_steps:%d",max_steps);
    printf("\r\nMotor init ...\n");
    vvtk_motor_init();

    printf("Motor pan set ORIGIN...\n");
    action.direction=VVTK_MOTOR_DIRECTION_ORIGIN;
    action.speed=50;
    action.step=1000;
    vvtk_motor(VVTK_MOTOR_PAN, &action);
    //sleep(10);

    printf("Motor tilt set ORIGIN...\n");
    action.direction=VVTK_MOTOR_DIRECTION_ORIGIN;
    action.speed=50;
    action.step=720;
    vvtk_motor(VVTK_MOTOR_TILT, &action);
    //sleep(10);

    printf("TILT MOTOR\n");
    vvtk_set_motor_status_callback(VVTK_MOTOR_TILT,tilt_cb,user_data_tilt);
    sleep(2);
	while(g_tilt_status.status != VVTK_MOTOR_STATUS_STOP){
		usleep(6000);
	}

    printf("Motor tilt set UP...\n");
    action.direction=VVTK_MOTOR_DIRECTION_UP;
    action.speed=50;
    action.step=500;
    vvtk_motor(VVTK_MOTOR_TILT, &action);
    sleep(3);

    printf("Motor tilt set DOWN...\n");
    action.direction=VVTK_MOTOR_DIRECTION_DOWN;
    action.speed=50;
    action.step=500;
    vvtk_motor(VVTK_MOTOR_TILT, &action);
    sleep(1);
	while(g_tilt_status.status != VVTK_MOTOR_STATUS_STOP){
		usleep(6000);
	}
    CU_ASSERT(g_tilt_status.step == 0);
    vvtk_set_motor_status_callback(VVTK_MOTOR_TILT,zero_cb,user_data_zero);
    sleep(2);

    printf("PAN MOTOR\n");

    vvtk_set_motor_status_callback(VVTK_MOTOR_PAN,pan_cb,user_data_pan);
    sleep(2);
	while(g_pan_status.status != VVTK_MOTOR_STATUS_STOP){
		usleep(6000);
	}
    printf("Motor pan set LEFT...\n");
    action.direction=VVTK_MOTOR_DIRECTION_LEFT;
    action.speed=50;
    action.step=500;
    vvtk_motor(VVTK_MOTOR_PAN, &action);
    sleep(3);

    printf("Motor pan set RIGHT...\n");
    action.direction=VVTK_MOTOR_DIRECTION_RIGHT;
    action.speed=50;
    action.step=500;
    vvtk_motor(VVTK_MOTOR_PAN, &action);
    sleep(1);
	while(g_pan_status.status != VVTK_MOTOR_STATUS_STOP){
		usleep(6000);
	}
    CU_ASSERT(g_pan_status.step == 0);
    vvtk_set_motor_status_callback(VVTK_MOTOR_PAN,zero_cb,user_data_zero);
    sleep(2);
#if 0
    vvtk_set_motor_status_callback(VVTK_MOTOR_PAN,pan_cb,user_data_pan);
    //pan
    printf("Motor pan set LEFT...\n");
    action.direction=VVTK_MOTOR_DIRECTION_LEFT;
    action.speed=50;
    action.step=1000;
    vvtk_motor(VVTK_MOTOR_PAN, &action);
    sleep(2);

    printf("Motor pan set stop...\n");
    action.direction=VVTK_MOTOR_DIRECTION_LEFT;
    action.speed=0;
    action.step=0;
    vvtk_motor(VVTK_MOTOR_PAN, &action);
    sleep(5);

    printf("Motor pan set RIGHT...\n");
    action.direction=VVTK_MOTOR_DIRECTION_RIGHT;
    action.speed=50;
    action.step=1000;
    vvtk_motor(VVTK_MOTOR_PAN, &action);
    sleep(2);

    printf("Motor pan set stop...\n");
    action.direction=VVTK_MOTOR_DIRECTION_RIGHT;
    action.speed=0;
    action.step=0;
    vvtk_motor(VVTK_MOTOR_PAN, &action);
    sleep(5);
#endif
}

void test_sdcard(void){
    int ret=-1, check_format=-1;

    char sd_size[64]={0};
    char user_data_sdcard[128]="SDcard Test";
    vvtk_sdcard_info sdinfo={0};

    vvtk_set_sd_insert_callback(sdcard_change_callback,user_data_sdcard);
    printf("\r\n[%s]please insert sdcard in 20s.\n",__FUNCTION__);
    ret = detect_stdin();
    CU_ASSERT(ret == 0);
    if(ret < 0){
        printf("\r\n[%s] timeout.\n",__FUNCTION__);
        return 1;
    }
    CU_ASSERT(test_sdcard_callback > 0);

    printf("\r\n[%s]start sdcard formating, it takes some times.\n",__FUNCTION__);
    check_format=vvtk_sdcard_format(VVTK_FS_EXT4);
    if(check_format==0){
        printf("\r\n[%s]sdcard format done. \n",__FUNCTION__);
    }
    CU_ASSERT(check_format == 0);
    sleep(5);

    printf("\r\n[%s]please re-plug sdcard to re-mount in 20s.\n",__FUNCTION__);
    ret = detect_stdin();
    CU_ASSERT(ret == 0);
    if(ret < 0){
        printf("\r\n[%s] timeout.\n",__FUNCTION__);
        return 1;
    }

    vvtk_get_sdcard_info(&sdinfo);
    printf("\r\n[%s]sdinfo total_size:%lld, remaining_capacity:%lld\n",__FUNCTION__, sdinfo.total_size, sdinfo.remaining_capacity);

    sprintf(sd_size, "%064d", sdinfo.total_size);
    CU_ASSERT(strlen(sd_size) > 2);

}

void test_vvtk_led_lighting()
{
    int value;
    int ret;
    value=1;
    ret=vvtk_set_led_lighting(VVTK_LIGHTING_LED_IR,value);
    CU_ASSERT(ret==VVTK_RET_ERR_NOT_SUPPORTED);

    ret=vvtk_get_led_lighting(VVTK_LIGHTING_LED_IR,&value);
    CU_ASSERT(ret==VVTK_RET_ERR_NOT_SUPPORTED);

    value = 1;
    ret=vvtk_set_led_lighting(VVTK_LIGHTING_LED_WHITE,value);
    CU_ASSERT(ret==VVTK_RET_SUCCESS);

    ret=vvtk_get_led_lighting(VVTK_LIGHTING_LED_WHITE,&value);
    CU_ASSERT(ret==VVTK_RET_SUCCESS);
    CU_ASSERT(value==1);

    sleep(1);
    value = 0;
    ret=vvtk_set_led_lighting(VVTK_LIGHTING_LED_WHITE,value);
    CU_ASSERT(ret==VVTK_RET_SUCCESS);

    ret=vvtk_get_led_lighting(VVTK_LIGHTING_LED_WHITE,&value);
    CU_ASSERT(ret==VVTK_RET_SUCCESS);
    CU_ASSERT(value==0);

}


void test_vvtk_get_light_sensor()
{
    int lightsensor_value=-1;
    int ret;

    ret=vvtk_get_light_sensor(&lightsensor_value);
    printf("\r\n[%s] light sensor value:%d\n",__FUNCTION__, lightsensor_value);
    CU_ASSERT(ret==VVTK_RET_SUCCESS);
    CU_ASSERT(lightsensor_value>=0);
}
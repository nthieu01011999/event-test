#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <CUnit/Basic.h>

#include <vvtk_camera.h>
#include <vvtk_event.h>
#include <vvtk_io.h>

static char *usr_data;
static vvtk_motor_status_t pan_status={0}, tilt_status={0};

VVTK_MOTOR_CALLBACK zerocb(vvtk_motor_status_t status, const void *user_data){
    usr_data = user_data;
}

VVTK_MOTOR_CALLBACK pancb(vvtk_motor_status_t status, const void *user_data)
{
	usr_data = user_data;
	printf("\n [pan]usr_data = %s \n",usr_data);
	pan_status.step = status.step;
	pan_status.status = status.status;
	printf("\n step = %d, status.status=%d \n",status.step, status.status);
}

VVTK_MOTOR_CALLBACK tiltcb(vvtk_motor_status_t status, const void *user_data)
{
	usr_data = user_data;
	printf("\n [tilt]usr_data = %s \n",usr_data);
	tilt_status.step = status.step;
	tilt_status.status = status.status;
	printf("\n step = %d, status.status=%d \n",status.step, status.status);
}

int main(int argc, char *argv[]) {
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
    vvtk_set_motor_status_callback(VVTK_MOTOR_PAN,pancb,user_data_pan);
    sleep(2);
	while(pan_status.status != VVTK_MOTOR_STATUS_STOP){
		usleep(6000);
	}

    sleep(5);

    printf("Motor tilt set ORIGIN...\n");
    action.direction=VVTK_MOTOR_DIRECTION_ORIGIN;
    action.speed=50;
    action.step=720;
    vvtk_motor(VVTK_MOTOR_TILT, &action);
    // sleep(10);
    vvtk_set_motor_status_callback(VVTK_MOTOR_TILT,tiltcb,user_data_tilt);
    while(tilt_status.status != VVTK_MOTOR_STATUS_STOP){
		usleep(6000);
	}

    // while (1) {

    // }


	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <CUnit/Basic.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include "vvtk_system.h"
#include "test_system_api.h"

void test_system_access_sn(void)
{
    char sn[128], tmp_sn[128];
    sprintf(sn, "2220-I03L-R17-03024020000015");
    system("fw_setenv serial_number 2220-I03L-R17-03024020000015");
    vvtk_system_get_sn(tmp_sn, 128);
    CU_ASSERT_STRING_EQUAL(sn, tmp_sn);
}

int check_macaddr(const char *mac)
{
    int i = 0;
    int s = 0;

    while (*mac)
    {
        if (isxdigit(*mac))
        {
            i++;
        }
        else if (*mac == ':' || *mac == '-')
        {
            if (i == 0 || i / 2 - 1 != s)
                break;
            ++s;
        }
        else
        {
            s = -1;
        }
        ++mac;
    }
    return (i == 12 && (s == 5 || s == 0));
}

void test_system_get_mac(void)
{
    char tmp_mac[32];
    vvtk_system_get_mac(tmp_mac, 32);
    CU_ASSERT(check_macaddr(tmp_mac));
}

void test_system_get_hw_info(void)
{
    char tmp_hwinfo[512];
    memset(tmp_hwinfo,0x0,512);
    vvtk_system_get_hw_info(tmp_hwinfo, 512);
    printf("\n%s\n",tmp_hwinfo);
    CU_ASSERT(strstr(tmp_hwinfo,"mb,led")!=NULL);    
}

void test_system_start_network()
{
    char ip[16];
    memset(ip,0x0,16);
    int cnt=0;
    int ret=0;
    ret=vvtk_system_start_network(VVTK_NETIF_ETH);

    if(ret!=VVTK_RET_SUCCESS){
        printf("ret=%d\n",ret);
        goto error_exit;
    }

    while(1){
        if(vvtk_system_get_network_ip(VVTK_NETIF_ETH,ip,16)==VVTK_RET_SUCCESS){
            if(strlen(ip)>=7){
                printf("ip=%s\n",ip);
                break;
            }
        }
        sleep(1);
        cnt++;
        if(cnt>=10){
            goto error_exit;
        }
    }
     CU_ASSERT(1);
     return;
error_exit:
     CU_ASSERT(0);
    return;
}

void test_system_fw_upgrade()
{
    char *filepath="/tmp/fwimage";
    int ret;
    ret=vvtk_system_fw_ota(filepath);
    printf("ret=%d\n",ret);
}

void test_system_get_capability()
{
    char capability[1024];
    int ret=0;
    memset(capability,0x0,1024);
    ret=vvtk_system_get_capability(capability,64);
    CU_ASSERT_EQUAL(ret,VVTK_RET_ERR_NO_MEM);

    ret=vvtk_system_get_capability(capability,1024);
    CU_ASSERT_EQUAL(ret,VVTK_RET_SUCCESS);
    if(ret!=0){
        printf("API return error :%d\n",ret);
    }
    printf("capability:\n%s\n",capability);
}

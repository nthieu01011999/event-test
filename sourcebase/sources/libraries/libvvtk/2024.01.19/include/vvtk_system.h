#ifndef __VVTK_SYSTEM_H__
#define __VVTK_SYSTEM_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include <vvtk_def.h>
#define MAX_SN_LENGTH 128

    /**
     * @brief Initiates a system reboot.
     *
     * This function is responsible for triggering a system reboot using the VVTK framework.
     * It performs the necessary operations to gracefully restart the system.
     *
     * @return VVTK_RET value indicating the success or failure of the reboot operation.
     *         Use this return value to handle errors or confirm successful reboot.
     */

    VVTK_RET vvtk_system_reboot();

    VVTK_RET vvtk_system_get_sn(char *sn, int len);
    VVTK_RET vvtk_system_set_sn(char *sn);
    VVTK_RET vvtk_system_get_mac(char *macaddr, int len);
    VVTK_RET vvtk_system_get_hw_info(char *hwinfo,int len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __VVTK_SYSTEM_H__ */

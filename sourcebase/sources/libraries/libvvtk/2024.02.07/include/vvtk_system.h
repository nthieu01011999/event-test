#ifndef __VVTK_SYSTEM_H__
#define __VVTK_SYSTEM_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include <vvtk_def.h>
#define MAX_SN_LENGTH 128

    typedef enum
    {
        VVTK_NETIF_ETH = 0,
        VVTK_NETIF_WIFI_STA,
        VVTK_NETIF_WIFI_AP,
    } VVTK_NETIF;

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
    /**
     * @brief Perform firmware Over-The-Air (OTA) update for the system.
     *
     * This function initiates an Over-The-Air update for the system firmware using the specified
     * file path. The file at the given path should contain the updated firmware image.
     *
     * @param filepath The path to the firmware image file for OTA update.
     * @return VVTK_RET_SUCCESS if the OTA update is successful, error code otherwise.
     */
    VVTK_RET vvtk_system_fw_ota(char *filepath);

    /**
     * @brief Perform application Over-The-Air (OTA) update for the system.
     *
     * This function initiates an Over-The-Air update for the system application using the specified
     * file path. The file at the given path should contain the updated application image.
     *
     * @param filepath The path to the application image file for OTA update.
     * @return VVTK_RET_SUCCESS if the OTA update is successful, error code otherwise.
     */
    VVTK_RET vvtk_system_app_ota(char *filepath);

    VVTK_RET vvtk_system_get_sn(char *sn, int len);
    VVTK_RET vvtk_system_set_sn(char *sn);
    VVTK_RET vvtk_system_get_mac(char *macaddr, int len);
    VVTK_RET vvtk_system_get_hw_info(char *hwinfo, int len);

    VVTK_RET vvtk_system_start_network(VVTK_NETIF netif);
    VVTK_RET vvtk_system_get_network_ip(VVTK_NETIF netif,char *ip, int len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __VVTK_SYSTEM_H__ */

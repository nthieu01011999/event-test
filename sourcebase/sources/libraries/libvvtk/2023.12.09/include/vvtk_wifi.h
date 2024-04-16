#ifndef __VVTK_WIFI_H__
#define __VVTK_WIFI_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include <vvtk_def.h>

#define MAX_SSID_NUM (32 * 4)

    typedef enum
    {
        VVTK_WIFI_SCAN_STATUS_UNKNOWN = 0,
        VVTK_WIFI_SCAN_STATUS_ONGOING,
        VVTK_WIFI_SCAN_STATUS_FINISHED,
    } VVTK_WIFI_SCAN_STATUS;

    // Define a structure to hold information about a Wi-Fi network
    typedef struct
    {
        char ssid[MAX_SSID_NUM + 1]; // SSID (Service Set Identifier) of the network
        char encrypt_mode[32];
        int signal_strength; // Signal strength in dBm
        unsigned char mac_address[6];
		// Add more fields as needed, e.g., encryption type, channel, etc.
    } wifi_network_info;

    /**
     * Initialize the Wi-Fi driver and application.
     *
     * @return VVTK_RET_SUCCESS if the Wi-Fi library is initialized successfully,
     *         or an error code if there are any issues during initialization.
     */

    VVTK_RET vvtk_wifi_init();

    /**
     * Set the Wi-Fi Access Point (AP) information.
     *
     * This function allows you to configure the SSID (Service Set Identifier) and password
     * for the Wi-Fi Access Point (AP) to which your device will connect.
     *
     * @param ssid   The SSID for the Wi-Fi Access Point.
     * @param passwd The password for the Wi-Fi Access Point.
     * @return VVTK_RET_SUCCESS if the Wi-Fi AP information is set successfully,
     *         or other VVTK_RET error(s) if there are any issues.
     */

    VVTK_RET vvtk_wifi_connect(const char *ssid, const char *passwd);

    /**
     * Start scanning for available Wi-Fi Access Points (APs).
     *
     * This function initiates a scan for nearby Wi-Fi Access Points that your
     * device can potentially connect to.
     *
     * @param scan_callback The callback function to handle scan results.
     * @return VVTK_RET_SUCCESS if the scan is started successfully,
     *         or other VVTK_RET error(s) if there are any issues.
     */
    VVTK_RET vvtk_wifi_start_scan(void (*scan_callback)(wifi_network_info *networks, int num_networks));

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __VVTK_WIFI_H__ */

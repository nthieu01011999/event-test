#ifndef __VVTK_CAMERA_H__
#define __VVTK_CAMERA_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <vvtk_def.h>

    typedef enum
    {
        VVTK_ANTI_FLICKER_MODE_AUTO = 0,
        VVTK_ANTI_FLICKER_MODE_50_MHZ,
        VVTK_ANTI_FLICKER_MODE_60_MHZ,
    } VVTK_ANTI_FLICKER_MODE;

    typedef enum
    {
        VVTK_DAY_NIGHT_MODE_AUTO = 0,
        VVTK_DAY_MODE,
        VVTK_NIGHT_MODE,
    } VVTK_DAY_NIGHT_MODE;

    typedef struct
    {
        int brightness;
        int contrast;
        int saturation;
        int sharpness;
        int mirror;
        int flip;
    } vvtk_isp_config_t;

    typedef struct 
    {
        int minimum;
        int maximum;
    } vvtk_range_t;

    typedef struct
    {
        vvtk_range_t brightness;
        vvtk_range_t contrast;
        vvtk_range_t saturation;
        vvtk_range_t sharpness;
        vvtk_range_t mirror;
        vvtk_range_t flip;
    } vvtk_isp_config_range_t;

    typedef enum
    {
        VVTK_OSD_TOP_LEFT = 0,
        VVTK_OSD_BOTTOM_LEFT,
        VVTK_OSD_TOP_RIGHT,
        VVTK_OSD_BOTTOM_RIGHT,
    } VVTK_OSD_POSITION;


    /**
     * @brief Sets the anti-flicker mode for the camera.
     *
     * This function allows you to set the anti-flicker mode for the camera to reduce
     * flickering in the captured images. It takes a mode parameter of type
     * VVTK_ANTI_FLICKER_MODE to specify the desired mode.
     *
     * @param mode The anti-flicker mode to set.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_set_anti_flicker(VVTK_ANTI_FLICKER_MODE mode);

    /**
     * @brief Retrieves the current anti-flicker mode of the camera.
     *
     * This function allows you to retrieve the current anti-flicker mode of the camera.
     * It stores the mode in the provided mode pointer.
     *
     * @param mode Pointer to a VVTK_ANTI_FLICKER_MODE variable to store the mode.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_get_anti_flicker(VVTK_ANTI_FLICKER_MODE *mode);

    /**
     * @brief Retrieves the current ISP (Image Signal Processor) configuration.
     *
     * This function retrieves the current image processing parameters such as
     * brightness, contrast, saturation, sharpness, mirror, and flip. The configuration
     * is stored in the provided config structure.
     *
     * @param config Pointer to a vvtk_isp_config_t structure to store the ISP configuration.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_get_isp_config(vvtk_isp_config_t *config);

    /**
     * @brief Retrieves the current ISP (Image Signal Processor) configuration range.
     *
     * This function retrieves the current image processing parameters range such as
     * brightness, contrast, saturation, sharpness, mirror, and flip. The configuration
     * range date is stored in the provided structure.
     *
     * @param config_range Pointer to a vvtk_isp_config_range_t structure to store config data range.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_get_isp_config_range(vvtk_isp_config_range_t *config_range);

    /**
     * @brief Sets the ISP (Image Signal Processor) configuration for the camera.
     *
     * This function allows you to set the image processing parameters such as brightness,
     * contrast, saturation, sharpness, mirror, and flip for the camera. It takes a
     * configuration structure as input.
     *
     * @param config Pointer to a vvtk_isp_config_t structure containing the ISP configuration.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_set_isp_config(vvtk_isp_config_t *config);

    /**
     * @brief Sets the day/night mode for the camera.
     *
     * This function allows you to set the day/night mode for the camera, which can
     * be set to auto, day mode, or night mode. It takes a mode parameter of type
     * VVTK_DAY_NIGHT_MODE to specify the desired mode.
     *
     * @param mode The day/night mode to set.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_set_day_night_mode(VVTK_DAY_NIGHT_MODE mode);

    /**
     * @brief Retrieves the current day/night mode of the camera.
     *
     * This function allows you to retrieve the current day/night mode of the camera.
     * It stores the mode in the provided mode pointer.
     *
     * @param mode Pointer to a VVTK_DAY_NIGHT_MODE variable to store the mode.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_get_day_night_mode(VVTK_DAY_NIGHT_MODE *mode);
    /**
     * @brief Sets the position of the on-screen display (OSD) for displaying time.
     *
     * This function allows you to set the position where the time should be displayed
     * on the camera's video feed. The position is specified using the VVTK_OSD_POSITION
     * enum.
     *
     * @param position The position at which the OSD time should be displayed.
     * @return VVTK_RET indicating the success or failure of the operation.
     */

    VVTK_RET vvtk_set_osd_time(VVTK_OSD_POSITION position);
    /**
     * @brief Retrieves the current position of the on-screen display (OSD) for displaying time.
     *
     * This function allows you to retrieve the current position where the time is displayed
     * on the camera's video feed. The current position is stored in the provided position
     * pointer.
     *
     * @param position Pointer to a VVTK_OSD_POSITION variable to store the current OSD position.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_get_osd_time(VVTK_OSD_POSITION *position);
    /**
     * @brief Sets the position of the on-screen display (OSD) for displaying custom text.
     *
     * This function allows you to set the position where custom text should be displayed
     * on the camera's video feed. The position is specified using the VVTK_OSD_POSITION
     * enum.
     *
     * @param position The position at which the OSD custom text should be displayed.
     * @param text The OSD custom text to displayed. 
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_set_osd_text(VVTK_OSD_POSITION position, char *text);
    /**
     * @brief Retrieves the current position of the on-screen display (OSD) for displaying custom text.
     *
     * This function allows you to retrieve the current position where custom text is displayed
     * on the camera's video feed. The current position is stored in the provided position
     * pointer.
     *
     * @param position Pointer to a VVTK_OSD_POSITION variable to store the current OSD position.
     * @param text Pointer to a variable to store the current OSD text.
     * @param text_len The size of the text variable to store the current OSD text.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_get_osd_text(VVTK_OSD_POSITION *position, char *text, int text_len);
    /**
     * @brief Sets the privacy mask configuration for a specific window.
     *
     * This function allows you to set the privacy mask configuration for a specific window
     * on the camera's video feed. You can specify the index of the window, enable or disable
     * the privacy mask, and provide the window configuration using the vvtk_window_config_t
     * structure.
     *
     * @param index The index of the privacy mask window.
     * @param enable Flag to enable or disable the privacy mask for the specified window.
     * @param window The configuration for the privacy mask window.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_set_privacy_mask(int index, int enable, vvtk_window_config_t *window);
    /**
     * @brief Retrieves the privacy mask configuration for a specific window.
     *
     * This function allows you to retrieve the privacy mask configuration for a specific window
     * on the camera's video feed. You can specify the index of the window, and the enable status
     * and window configuration are stored in the provided pointers.
     *
     * @param index The index of the privacy mask window.
     * @param enable Pointer to an integer to store the enable status of the privacy mask for
     *               the specified window.
     * @param window Pointer to a vvtk_window_config_t structure to store the configuration
     *               for the privacy mask window.
     * @return VVTK_RET indicating the success or failure of the operation.
     */
    VVTK_RET vvtk_get_privacy_mask(int index, int *enable, vvtk_window_config_t *window);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __VVTK_CAMERA_H__ */

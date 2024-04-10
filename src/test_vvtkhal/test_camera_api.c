
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include<sys/time.h>
#include<unistd.h>
#include <CUnit/Basic.h>

#include "vvtk_camera.h"

void test_vvtk_isp_config(void)
{
    vvtk_isp_config_t config;
    vvtk_isp_config_range_t config_range;

    CU_ASSERT(0==vvtk_get_isp_config_range(&config_range));

    CU_ASSERT(0==vvtk_get_isp_config(&config));

    config.brightness = config_range.brightness.maximum;
    config.contrast = config_range.contrast.maximum;
    config.saturation = config_range.saturation.maximum;
    config.sharpness = config_range.sharpness.maximum;
    config.mirror = config_range.mirror.maximum;
    config.flip = config_range.mirror.maximum;
    CU_ASSERT(0==vvtk_set_isp_config(&config));

    memset(&config, 0, sizeof(config));
    CU_ASSERT(0==vvtk_get_isp_config(&config));

    CU_ASSERT(config.brightness==config_range.brightness.maximum);
    CU_ASSERT(config.contrast==config_range.contrast.maximum);
    CU_ASSERT(config.saturation==config_range.saturation.maximum);
    CU_ASSERT(config.sharpness==config_range.sharpness.maximum);
    CU_ASSERT(config.mirror==config_range.mirror.maximum);
    CU_ASSERT(config.flip==config_range.flip.maximum);

    config.brightness = config_range.brightness.minimum;
    config.contrast = config_range.contrast.minimum;
    config.saturation = config_range.saturation.minimum;
    config.sharpness = config_range.sharpness.minimum;
    config.mirror = config_range.mirror.minimum;
    config.flip = config_range.mirror.minimum;
    CU_ASSERT(0==vvtk_set_isp_config(&config));

    memset(&config, 0, sizeof(config));
    CU_ASSERT(0==vvtk_get_isp_config(&config));

    CU_ASSERT(config.brightness==config_range.brightness.minimum);
    CU_ASSERT(config.contrast==config_range.contrast.minimum);
    CU_ASSERT(config.saturation==config_range.saturation.minimum);
    CU_ASSERT(config.sharpness==config_range.sharpness.minimum);
    CU_ASSERT(config.mirror==config_range.mirror.minimum);
    CU_ASSERT(config.flip==config_range.flip.minimum);

    config.brightness = config_range.brightness.maximum+1;
    config.contrast = config_range.contrast.maximum+1;
    config.saturation = config_range.saturation.maximum+1;
    config.sharpness = config_range.sharpness.maximum+1;
    config.mirror = config_range.mirror.maximum;
    config.flip = config_range.mirror.maximum;
    CU_ASSERT(0==vvtk_set_isp_config(&config));

    memset(&config, 0, sizeof(config));
    CU_ASSERT(0==vvtk_get_isp_config(&config));

    CU_ASSERT(config.brightness != (config_range.brightness.maximum+1));
    CU_ASSERT(config.contrast != (config_range.contrast.maximum+1));
    CU_ASSERT(config.saturation != (config_range.saturation.maximum+1));
    CU_ASSERT(config.sharpness != (config_range.sharpness.maximum+1));

    config.mirror = config_range.mirror.maximum+1;
    config.flip = config_range.mirror.maximum+1;
    CU_ASSERT(0!=vvtk_set_isp_config(&config));

    config.brightness = (config_range.brightness.minimum + config_range.brightness.maximum) / 2;
    config.contrast = (config_range.contrast.minimum + config_range.contrast.maximum) / 2;
    config.saturation = (config_range.saturation.minimum + config_range.saturation.maximum) / 2;
    config.sharpness = (config_range.sharpness.minimum + config_range.sharpness.maximum) / 2;
    config.mirror = config_range.mirror.minimum;
    config.flip = config_range.mirror.minimum;
    CU_ASSERT(0==vvtk_set_isp_config(&config));
}

void test_vvtk_daynight_mode(void)
{
    VVTK_DAY_NIGHT_MODE mode;

    CU_ASSERT(0==vvtk_get_day_night_mode(&mode));

    mode = VVTK_DAY_NIGHT_MODE_AUTO;
    CU_ASSERT(0==vvtk_set_day_night_mode(mode));
    CU_ASSERT(0==vvtk_get_day_night_mode(&mode));
    CU_ASSERT(VVTK_DAY_NIGHT_MODE_AUTO==mode);

    sleep(60);

    mode = VVTK_DAY_MODE;
    CU_ASSERT(0==vvtk_set_day_night_mode(mode));
    CU_ASSERT(0==vvtk_get_day_night_mode(&mode));
    CU_ASSERT(VVTK_DAY_MODE==mode);

    sleep(5);

    mode = VVTK_NIGHT_MODE;
    CU_ASSERT(0==vvtk_set_day_night_mode(mode));
    CU_ASSERT(0==vvtk_get_day_night_mode(&mode));
    CU_ASSERT(VVTK_NIGHT_MODE==mode);

    sleep(5);

    mode = VVTK_DAY_MODE;
    CU_ASSERT(0==vvtk_set_day_night_mode(mode));
    CU_ASSERT(0==vvtk_get_day_night_mode(&mode));
    CU_ASSERT(VVTK_DAY_MODE==mode);

    CU_ASSERT(0==vvtk_day_night_mode_resource_release());
}


VVTK_RET_CALLBACK daynight_callback(VVTK_DAY_NIGHT_STATE state, const void *user_data)
{
    // printf("[%s][%d] state:%s user_data:%p\n", __func__, __LINE__, state == 1 ? "night":"day", user_data);
}

void test_vvtk_daynight_mode_ex(void)
{
    VVTK_DAY_NIGHT_MODE mode;

    printf("[%s][%d] VVTK_DAY_MODE\n", __func__, __LINE__);
    mode = VVTK_DAY_MODE;
    CU_ASSERT(0==vvtk_set_day_night_mode_ex(mode, daynight_callback, NULL));
    sleep(60);

    printf("[%s][%d] VVTK_NIGHT_MODE\n", __func__, __LINE__);
    mode = VVTK_NIGHT_MODE;
    CU_ASSERT(0==vvtk_set_day_night_mode_ex(mode, daynight_callback, NULL));
    sleep(60);

    printf("[%s][%d] VVTK_DAY_NIGHT_MODE_AUTO\n", __func__, __LINE__);
    mode = VVTK_DAY_NIGHT_MODE_AUTO;
    CU_ASSERT(0==vvtk_set_day_night_mode_ex(mode, daynight_callback, NULL));
    sleep(60);

    CU_ASSERT(0==vvtk_day_night_mode_resource_release());
}

void test_vvtk_anti_flicker(void)
{
    VVTK_ANTI_FLICKER_MODE mode;

    CU_ASSERT(0==vvtk_get_anti_flicker(&mode));

    mode = VVTK_ANTI_FLICKER_MODE_50_MHZ;
    CU_ASSERT(0==vvtk_set_anti_flicker(mode));
    CU_ASSERT(0==vvtk_get_anti_flicker(&mode));

    mode = VVTK_ANTI_FLICKER_MODE_60_MHZ;
    CU_ASSERT(0==vvtk_set_anti_flicker(mode));
    CU_ASSERT(0==vvtk_get_anti_flicker(&mode));

    mode = VVTK_ANTI_FLICKER_MODE_AUTO;
    CU_ASSERT(0==vvtk_set_anti_flicker(mode));
    CU_ASSERT(0==vvtk_get_anti_flicker(&mode));
}

void test_vvtk_privacy_mask(void)
{
    vvtk_window_config_t mask_window;
    int enable;

    mask_window.left_top.x = 0;
    mask_window.left_top.y = 0;
    mask_window.right_bottom.x = 100;
    mask_window.right_bottom.y = 100;
    CU_ASSERT(0==vvtk_set_privacy_mask(0, 1, &mask_window));

    sleep(5);
    mask_window.left_top.x = 1820;
    mask_window.left_top.y = 980;
    mask_window.right_bottom.x = 1920;
    mask_window.right_bottom.y = 1080;
    CU_ASSERT(0==vvtk_set_privacy_mask(3, 1, &mask_window));

    mask_window.left_top.x = 1820;
    mask_window.left_top.y = 0;
    mask_window.right_bottom.x = 1920;
    mask_window.right_bottom.y = 100;
    CU_ASSERT(0==vvtk_set_privacy_mask(1, 1, &mask_window));

    mask_window.left_top.x = 0;
    mask_window.left_top.y = 980;
    mask_window.right_bottom.x = 100;
    mask_window.right_bottom.y = 1080;
    CU_ASSERT(0==vvtk_set_privacy_mask(2, 1, &mask_window));
    sleep(5);

    CU_ASSERT(0==vvtk_get_privacy_mask(0, &enable, &mask_window));
    CU_ASSERT(0==vvtk_set_privacy_mask(0, 0, &mask_window));

    CU_ASSERT(0==vvtk_get_privacy_mask(1, &enable, &mask_window));
    CU_ASSERT(0==vvtk_set_privacy_mask(1, 0, &mask_window));

    CU_ASSERT(0==vvtk_get_privacy_mask(2, &enable, &mask_window));
    CU_ASSERT(0==vvtk_set_privacy_mask(2, 0, &mask_window));

    CU_ASSERT(0==vvtk_get_privacy_mask(3, &enable, &mask_window));
    CU_ASSERT(0==vvtk_set_privacy_mask(3, 0, &mask_window));
    sleep(5);
}

void test_vvtk_osd_time_pos(void)
{
    VVTK_OSD_POSITION pos = VVTK_OSD_TOP_LEFT;
    VVTK_OSD_POSITION pos2 = VVTK_OSD_TOP_LEFT;
    char buffer[64];


    pos = VVTK_OSD_TOP_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    sleep(5);

    pos = VVTK_OSD_BOTTOM_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_get_osd_time(&pos2));
    CU_ASSERT(pos == pos2);
    sleep(5);

    pos = VVTK_OSD_TOP_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_get_osd_time(&pos2));
    CU_ASSERT(pos == pos2);
    sleep(5);

    pos = VVTK_OSD_BOTTOM_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_get_osd_time(&pos2));
    CU_ASSERT(pos == pos2);
    sleep(5);

    pos = VVTK_OSD_TOP_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_get_osd_time(&pos2));
    CU_ASSERT(pos == pos2);
    sleep(5);

    pos = VVTK_OSD_BOTTOM_LEFT;
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890"));
    sleep(5);

    pos = VVTK_OSD_TOP_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    sleep(5);

    pos = VVTK_OSD_BOTTOM_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    sleep(5);

    pos = VVTK_OSD_TOP_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    sleep(5);

    pos = VVTK_OSD_BOTTOM_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    CU_ASSERT(0==vvtk_get_osd_text(&pos2, buffer, sizeof(buffer)));
    sleep(5);

    pos = VVTK_OSD_BOTTOM_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));

    pos = VVTK_OSD_BOTTOM_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    CU_ASSERT(0==vvtk_get_osd_text(&pos2, buffer, sizeof(buffer)));
    sleep(5);

    pos = VVTK_OSD_BOTTOM_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    
    pos = VVTK_OSD_BOTTOM_LEFT;
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    CU_ASSERT(0==vvtk_get_osd_text(&pos2, buffer, sizeof(buffer)));
    sleep(5);

    pos = VVTK_OSD_TOP_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    
    pos = VVTK_OSD_TOP_LEFT;
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    CU_ASSERT(0==vvtk_get_osd_text(&pos2, buffer, sizeof(buffer)));
    sleep(5);

    pos = VVTK_OSD_TOP_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    
    pos = VVTK_OSD_TOP_RIGHT;
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    CU_ASSERT(0==vvtk_get_osd_text(&pos2, buffer, sizeof(buffer)));
    sleep(5);

    pos = VVTK_OSD_BOTTOM_LEFT;
    CU_ASSERT(0==vvtk_set_osd_text(pos, "12345678901234567890123"));
    CU_ASSERT(0==vvtk_get_osd_text(&pos2, buffer, sizeof(buffer)));
    sleep(5);

    pos = VVTK_OSD_TOP_LEFT;
    CU_ASSERT(0==vvtk_set_osd_text(pos, ""));
    sleep(5);

    pos = VVTK_OSD_TOP_LEFT;
    CU_ASSERT(0==vvtk_set_osd_time(pos));
    sleep(5);
}

void test_vvtk_wb_config(void)
{
    VVTK_OSD_POSITION text_pos = VVTK_OSD_TOP_RIGHT;
    VVTK_AWB_MODE mode;
    vvtk_wb_temp_t wb_temp;

    vvtk_get_wb_mode(&mode);
    vvtk_get_wb_temperature(&wb_temp);
    // printf("[%s][%d] wb mode:%d\n", __func__, __LINE__, mode);
    // printf("[%s][%d] wb temp:%d (%d ~ %d)\n", __func__, __LINE__, wb_temp.value, wb_temp.range.minimum, wb_temp.range.maximum);

    mode = VVTK_AWB_MANUAL;
    vvtk_set_wb_mode(mode);

    wb_temp.value = 1700;
    vvtk_set_wb_temperature(&wb_temp);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "AWB-Man-1700"));
    sleep(10);

    wb_temp.value = 6500;
    vvtk_set_wb_temperature(&wb_temp);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "AWB-Man-6500"));
    sleep(10);

    mode = VVTK_AWB_AUTO;
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "AWB-Auto"));
    vvtk_set_wb_mode(mode);
    sleep(10);

    CU_ASSERT(0==vvtk_set_osd_text(text_pos, ""));
}

void test_vvtk_wdr_config(void)
{
    VVTK_OSD_POSITION text_pos = VVTK_OSD_TOP_RIGHT;
    VVTK_WDR_MODE mode;
    vvtk_wdr_level_t wdr_level;

    vvtk_get_wdr_mode(&mode);

    mode = VVTK_WDR_MANUAL;
    vvtk_set_wdr_mode(mode);

    vvtk_get_wdr_level(&wdr_level);

    wdr_level.value = wdr_level.range.maximum;
    vvtk_set_wdr_level(&wdr_level);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "WDR-Man-max"));
    sleep(10);

    wdr_level.value = wdr_level.range.minimum;
    vvtk_set_wdr_level(&wdr_level);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "WDR-Man-min"));
    sleep(10);

    wdr_level.value = wdr_level.range.maximum/2;
    vvtk_set_wdr_level(&wdr_level);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "WDR-Man-0.5max"));
    sleep(10);

    mode = VVTK_WDR_AUTO;
    vvtk_set_wdr_mode(mode);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "WDR-Auto"));
    sleep(10);

    mode = VVTK_WDR_DISABLE;
    vvtk_set_wdr_mode(mode);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "WDR-Disable"));
    sleep(10);

    CU_ASSERT(0==vvtk_set_osd_text(text_pos, ""));
}

void test_vvtk_dnr_config(void)
{
    VVTK_OSD_POSITION text_pos = VVTK_OSD_TOP_RIGHT;
    VVTK_DNR_MODE mode;

    vvtk_get_dnr(&mode);
    // printf("[%s][%d] dnr mode:%d\n", __func__, __LINE__, mode);

    mode = VVTK_DNR_ENABLE;
    vvtk_set_dnr(mode);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "DNR-Enable"));
    sleep(10);

    mode = VVTK_DNR_DISABLE;
    vvtk_set_dnr(mode);
    CU_ASSERT(0==vvtk_set_osd_text(text_pos, "DNR-Disable"));
    sleep(10);

    CU_ASSERT(0==vvtk_set_osd_text(text_pos, ""));
}
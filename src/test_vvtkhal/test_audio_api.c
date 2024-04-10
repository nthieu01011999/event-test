
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include<sys/time.h>
#include<unistd.h>
#include <CUnit/Basic.h>

#include "vvtk_audio.h"
#include "analyze_audio.h"

void test_vvtk_audio_output_volume(void)
{
    VVTK_AUDIO_VOLUME audio_volume;

    CU_ASSERT(0==vvtk_get_audio_output_volume(&audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_50;
    CU_ASSERT(0==vvtk_set_audio_output_volume(audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_0;
    CU_ASSERT(0==vvtk_get_audio_output_volume(&audio_volume));
    CU_ASSERT(VVTK_AUDIO_VOLUME_50==audio_volume);

    audio_volume = VVTK_AUDIO_VOLUME_0;
    CU_ASSERT(0==vvtk_set_audio_output_volume(audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_100;
    CU_ASSERT(0==vvtk_get_audio_output_volume(&audio_volume));
    CU_ASSERT(VVTK_AUDIO_VOLUME_0==audio_volume);

    audio_volume = VVTK_AUDIO_VOLUME_100;
    CU_ASSERT(0==vvtk_set_audio_output_volume(audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_0;
    CU_ASSERT(0==vvtk_get_audio_output_volume(&audio_volume));
    CU_ASSERT(VVTK_AUDIO_VOLUME_100==audio_volume);

    audio_volume = VVTK_AUDIO_VOLUME_80;
    CU_ASSERT(0==vvtk_set_audio_output_volume(audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_0;
    CU_ASSERT(0==vvtk_get_audio_output_volume(&audio_volume));
    CU_ASSERT(VVTK_AUDIO_VOLUME_80==audio_volume);
}

void test_vvtk_audio_input_volume(void)
{
    VVTK_AUDIO_VOLUME audio_volume;

    CU_ASSERT(0==vvtk_get_audio_input_volume(&audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_50;
    CU_ASSERT(0==vvtk_set_audio_input_volume(audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_0;
    CU_ASSERT(0==vvtk_get_audio_input_volume(&audio_volume));
    CU_ASSERT(VVTK_AUDIO_VOLUME_50==audio_volume);

    audio_volume = VVTK_AUDIO_VOLUME_100;
    CU_ASSERT(0==vvtk_set_audio_input_volume(audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_0;
    CU_ASSERT(0==vvtk_get_audio_input_volume(&audio_volume));
    CU_ASSERT(VVTK_AUDIO_VOLUME_100==audio_volume);

    audio_volume = VVTK_AUDIO_VOLUME_80;
    CU_ASSERT(0==vvtk_set_audio_input_volume(audio_volume));

    audio_volume = VVTK_AUDIO_VOLUME_0;
    CU_ASSERT(0==vvtk_get_audio_input_volume(&audio_volume));
    CU_ASSERT(VVTK_AUDIO_VOLUME_80==audio_volume);
}

void test_vvtk_audio_config(void)
{
    vvtk_audio_config_t config;

    //index 0 for g711
    CU_ASSERT(0==vvtk_get_audio_config(0, &config));

    config.codec = VVTK_AUDIO_CODEC_G711_ALAW;
    CU_ASSERT(0==vvtk_set_audio_config(0, &config));

    config.codec = VVTK_AUDIO_CODEC_G711_ULAW;
    CU_ASSERT(0==vvtk_set_audio_config(0, &config));

    //index 1 for AAC
    CU_ASSERT(0==vvtk_get_audio_config(1, &config));

    config.codec = VVTK_AUDIO_CODEC_AAC;
    config.rate = VVTK_AUDIO_SAMPLE_RATE_44_1_KHZ;
    config.channel = 1;
    CU_ASSERT(0==vvtk_set_audio_config(1, &config));

    CU_ASSERT(0==vvtk_get_audio_config(1, &config));
    CU_ASSERT(VVTK_AUDIO_CODEC_AAC==config.codec);
    CU_ASSERT(VVTK_AUDIO_SAMPLE_RATE_44_1_KHZ==config.rate);
    CU_ASSERT(1==config.channel);

    config.codec = VVTK_AUDIO_CODEC_AAC;
    config.rate = VVTK_AUDIO_SAMPLE_RATE_8_KHZ;
    config.channel = 1;
    CU_ASSERT(0==vvtk_set_audio_config(1, &config));

    config.codec = VVTK_AUDIO_CODEC_AAC;
    config.rate = VVTK_AUDIO_SAMPLE_RATE_48_KHZ;
    config.channel = 2;
    CU_ASSERT(0==vvtk_set_audio_config(1, &config));
}


void dump_file(char *data, int data_len)
{
    FILE *fp = fopen("/tmp/audio.out", "a+b");

    if(fp) 
    {
        fwrite(data, 1, data_len, fp);
        fclose(fp);
    }
}

#define DUMP_FILE 0

int i = 0;
VVTK_RET_CALLBACK audio_callback(vvtk_audio_frame_t *audio_frame, const void *user_data)
{
    static int i = 0;
    // printf("---------------------------\n");
    // printf("codec:%d\n", audio_frame->codec);
    // printf("timestamp:%llu\n", audio_frame->timestamp);
    // printf("data:%p size:%d\n", audio_frame->data, audio_frame->size);
    // printf("sequence:%d\n\n", audio_frame->sequence);

    CU_ASSERT(audio_frame->size > 0);

#if DUMP_FILE
    dump_file(audio_frame->data, audio_frame->size);
#endif

    if(i++<10)
        return VVTK_RET_CALLBACK_CONTINUE;
    else
        return VVTK_RET_CALLBACK_STOP;

    
}

void test_vvtk_audio_callback(void)
{
    sleep(2);

    i = 0;
    CU_ASSERT(0==vvtk_set_audio_callback(0, audio_callback, NULL));
    sleep(15);

    i = 0;
    CU_ASSERT(0==vvtk_set_audio_callback(1, audio_callback, NULL));
    sleep(15);
}

void test_vvtk_audio_output(void)
{
    vvtk_audio_config_t config;
    FILE *fp = NULL;
	int head_length = 0;
	int data_size = 0;
    char buffer[1024];
    int ret = 0;

    fp = fopen("/tmp/test.wav", "rb");

    CU_ASSERT_FATAL(fp!=NULL);
    CU_ASSERT_FATAL(0==analyze_audio_header(fp, &config, &head_length, &data_size));
    fseek(fp, head_length, SEEK_SET);

    CU_ASSERT(0==vvtk_create_audio_output(0, &config));

    while(1)
    {
        ret = fread(buffer, 1, 640, fp);

        vvtk_play_audio(0, buffer, ret);

        if(ret<640)
            break;
    }
    fclose(fp);

    CU_ASSERT(0==vvtk_stop_audio(0));
    CU_ASSERT(0==vvtk_destory_audio_output(0));
    sleep(2);
}


void test_vvtk_audio_file_playback(void)
{
    CU_ASSERT(0==vvtk_play_audio_file(0, "/tmp/test.wav"));

    sleep(20);//wait for playback audio
}

void test_vvtk_audio_g711_output(void)
{
    vvtk_audio_config_t config;
    FILE *fp = NULL;
	int head_length = 0;
	int data_size = 0;
    char buffer[1024];
    int ret = 0;

    fp = fopen("/tmp/test.alaw", "rb");

    CU_ASSERT_FATAL(fp!=NULL);

    fseek(fp, 0, SEEK_SET);

	config.codec = VVTK_AUDIO_CODEC_G711_ALAW;
	config.rate = VVTK_AUDIO_SAMPLE_RATE_8_KHZ;
	config.channel = 1;
	config.format = 16;

    CU_ASSERT(0==vvtk_create_audio_output(0, &config));

    while(1)
    {
        ret = fread(buffer, 1, 640, fp);

        vvtk_play_audio(0, buffer, ret);

        if(ret<640)
            break;
    }
    fclose(fp);

    CU_ASSERT(0==vvtk_stop_audio(0));
    CU_ASSERT(0==vvtk_destory_audio_output(0));
    sleep(2);
}
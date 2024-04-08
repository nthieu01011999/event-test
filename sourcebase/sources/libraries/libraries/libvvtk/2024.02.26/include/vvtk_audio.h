#ifndef __VVTK_AUDIO_H__
#define __VVTK_AUDIO_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <vvtk_def.h>

    typedef enum
    {
        VVTK_AUDIO_CODEC_AAC,
        VVTK_AUDIO_CODEC_G711_ALAW, /* G.711 A-law */
        VVTK_AUDIO_CODEC_G711_ULAW, /* G.711 μ-law */
        VVTK_AUDIO_CODEC_PCM,
    } VVTK_AUDIO_CODEC;

    typedef enum
    {
        VVTK_AUDIO_SAMPLE_RATE_8_KHZ = 0,
        VVTK_AUDIO_SAMPLE_RATE_16_KHZ,
        VVTK_AUDIO_SAMPLE_RATE_32_KHZ,
        VVTK_AUDIO_SAMPLE_RATE_44_1_KHZ,
        VVTK_AUDIO_SAMPLE_RATE_48_KHZ,
    } VVTK_AUDIO_SAMPLE_RATE;

    typedef struct
    {
        VVTK_AUDIO_CODEC codec;
        VVTK_AUDIO_SAMPLE_RATE rate;
        int channel;
        int format;
    } vvtk_audio_config_t;

    typedef struct
    {
        VVTK_BYTE *data;
        int size;
        VVTK_AUDIO_CODEC codec;
        VVTK_AUDIO_SAMPLE_RATE sample_rate;
        uint64_t timestamp;
        int sequence;
        int index;
    } vvtk_audio_frame_t;

    /**
     * Callback function declation of the audio usage
     *
     * @param audio_frame Audio frame
     * @return VVTK_RET_CALLBACK stop or continue
     */
    typedef VVTK_RET_CALLBACK (*VVTK_AUDIO_CALLBACK)(vvtk_audio_frame_t *audio_frame, const void *user_data);

    /**
     * Immediately to get the related parameters used for capturing
     *
     * @see vvtk_audio_config_t
     *
     * @param index Audio Stream Index (0, 1, or 2)
     * @param config The settings used for capturing and encoding the video stream
     * @return VVTK_RET_SUCCESS for setting succeed, VVTK_RET_NOT_SUPPORTED presents the requested configuration is not supported for this device or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_get_audio_config(int index, vvtk_audio_config_t *config);

    /**
     * Immediately to set the related parameters used for capturing
     *
     * @see vvtk_audio_config_t
     *
     * NOTICE: PCM_16KHZ / 48KHZ / AAC_16KHZ / AAC_48KHZ should be supported
     * @param index Audio Stream Index (0, 1, or 2)
     * @param config The settings used for capturing and encoding the video stream
     * @return VVTK_RET_SUCCESS for setting succeed, VVTK_RET_NOT_SUPPORTED presents the requested configuration is not supported for this device or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_set_audio_config(int index, const vvtk_audio_config_t *config);

    /**
     * Immediately and continuously to get audio data captured from the microphone by Callback
     *
     * @see vvtk_audio_config_t
     *
     * @param index Audio Steam Index (0 or 1)
     * @param cb The callback function when capturing the audio stream
     * @return VVTK_RET_SUCCESS with audio to be captured, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_set_audio_callback(int index, const VVTK_AUDIO_CALLBACK cb, const void *user_data);

    /**
     * Initialize the CODEC when playing audio data in the function, vvtk_play_audio_output
     *
     * @param index The index of the audio output
     * @param config The settings of the audio output, currently only support VVTK_AUDIO_CODEC_PCM format
     * @return VVTK_RET_SUCCESS without errors, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_create_audio_output(int index, const vvtk_audio_config_t *config);

    /**
     * Immediately to release the internal resource
     *
     * @param index The index of the audio output
     * @return VVTK_RET_SUCCESS without errors, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_destory_audio_output(int index);

    /**
     * Get sound level for audio output (0(Mute) to 100(Loudest))
     *
     * @param index The index of the audio output
     * @param volume The sound level (0(Mute) to 100(Loudest)) returned
     * @return VVTK_RET_SUCCESS with the volume value saved, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_get_audio_output_volume(int *volume);

    /**
     * Set sound level for audio output (0(Mute) to 100(Loudest))
     *
     * @param volume The sound level (0(Mute) to 100(Loudest))
     * @return VVTK_RET_SUCCESS with the volume value saved, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_set_audio_output_volume(int volume);

    /**
     * Immediately to play audio stream (Non-blocking)
     *
     * @param index The index of the audio output
     * @param buf The data of the audio stream
     * @param size The size of the audio data
     * @return VVTK_RET_SUCCESS without errors, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_play_audio(int index, const void *buf, int size);

    /**
     * Immediately to play the audio file supported by ALSA (Non-blocking)
     *
     * @param file_path the path of the audio file to be played
     * @return VVTK_RET_SUCCESS with the target Audio file played, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_play_audio_file(int index, const char *file_path);

    /**
     * Immediately to stop playing
     *
     * @return VVTK_RET_SUCCESS without errors, or other VVTK_RET error(s)
     */
    VVTK_RET vvtk_stop_audio(int index);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __VVTK_AUDIO_H__ */

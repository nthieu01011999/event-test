#ifndef __H_MTCE_RTMP__
#define __H_MTCE_RTMP__

#ifdef __cplusplus
extern "C"
{
#endif

    int rtmp_initialize();
    int rtmp_connect(const char *serial, const char *host, int port, const char *node, const char *token);
    int rtmp_stop();
    int rtmp_videoSend(unsigned char *data, int size, int frameType);
    int rtmp_audioSend(unsigned char *data, int size);

#ifdef __cplusplus
}
#endif
#endif // __H_MTCE_RTMP__
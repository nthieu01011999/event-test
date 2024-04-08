/**
****************************************************************************************
* @author: PND
* @date: 2024-03-14
* @file: mtce_s3.h
* @brief: source sdk
*****************************************************************************************
**/
#ifndef __H_MTCE_S3__
#define __H_MTCE_S3__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef enum
    {
        S3_EVENT_MOTION,
        S3_EVENT_HUMAN,
        S3_EVENT_FACE,
        S3_EVENT_NONE
    } S3_EVENT_TYPE_E;

    enum S3_E
    {
        S3_OK = 0,
        S3_FAILED_INIT = -100,
        S3_AK_FAIL,
        S3_HTTP_POST_FAIL,
        S3_HTTP_PUT_FAIL,
        S3_INVALID_ARG,
        S3_INVALID_URL,
        S3_INVALID_DATA,
        S3_INVALID_PARAMETER,
        S3_INVALID_SIZE,
        S3_INVALID_BUCKET,
        S3_ERR_OPEN,
        S3_ERR_WRITE,
        S3_ERR_READ,
        S3_ERR_CLOSE,
        S3_ERR_CONNECT,
        S3_DOWN_TIMEOUT,
        S3_NOT_RESPONSE,

    };

    /**
     * @brief Initialization S3
     *
     * @param cafile is SSL Certificate
     * @return  0 : success , minus : fail
     */
    int s3Init(const char *serial, const char *cafile);

    /**
     * @brief Set Access key
     *
     * @param accesskey is Access key S3
     * @param endpoint is endpoint server
     * @return  0 : success , minus : fail
     */
    int s3SetConf(const char *accesskey, const char *endpoint);

    /**
     * @brief Push image to S3 storage
     *
     * @param filePath   image file path
     * @param type       image type S3_EVENT_TYPE_E
     * @param timestamp  image time snap
     * @return 0 : success , minus : fail
     */
    int s3PutImage(const char *filePath, S3_EVENT_TYPE_E type, uint64_t timestamp);

    /**
     * @brief Push video mp4 to S3 storage
     *
     * @param filePath   mp4 file path
     * @param type       mp4 type S3_EVENT_TYPE_E
     * @param timestamp  mp4 time record
     * @return 0 : success , minus : fail
     */
    int s3PutVideoMp4(const char *filePath, S3_EVENT_TYPE_E type, uint64_t timestamp);


#ifdef __cplusplus
}
#endif

#endif // __H_MTCE_S3__
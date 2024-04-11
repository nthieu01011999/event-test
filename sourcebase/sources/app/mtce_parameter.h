/**
****************************************************************************************
* @author: PND
* @ Modified by: Your name
* @ Modified time: 2023-05-16 14:54:16
* @brief: mtce
*****************************************************************************************
**/
#ifndef __MTCE_PARAMETER_H__
#define __MTCE_PARAMETER_H__

#include <stdbool.h>

#include <vvtk_def.h>
#include <vvtk_video.h>
#include <vvtk_audio.h>



typedef struct {
	char serial[64];
} mtce_deviceInfo_t;


extern mtce_deviceInfo_t mtce_deviceInfo;

extern int mtce_contextInit();

#endif	  // __MTCE_PARAMETER_H__
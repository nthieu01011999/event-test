#ifndef __APP_DBG_H__
#define __APP_DBG_H__

#include <stdio.h>
#include "sys_dbg.h"

#define APP_DBG_EN	   1
#define APP_PRINT_EN   1
#define APP_ERR_EN	   1
#define APP_DBG_SIG_EN 1

/* module debug */
#define SD_DEBUG	 0
#define RECORD_DEBUG 0

#if (APP_PRINT_EN == 1)
#define APP_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)
#endif

#if (APP_DBG_EN == 1) && (RELEASE == 0)
#define APP_DBG(fmt, ...) __LOG__(fmt, "APP_DBG", ##__VA_ARGS__)
#else
#define APP_DBG(fmt, ...)
#endif

#if (APP_ERR_EN == 1)
#define APP_ERR(x...)                                         \
	do {                                                      \
		printf("\033[1;31m%s->%d: ", __FUNCTION__, __LINE__); \
		printf(x);                                            \
		printf("\033[0m\n");                                  \
	} while (0)
#else
#define APP_ERR(fmt, ...)
#endif

#if (APP_DBG_SIG_EN == 1)
#define APP_DBG_SIG(fmt, ...) __LOG__(fmt, "SIG -> ", ##__VA_ARGS__)
#else
#define APP_DBG_SIG(fmt, ...)
#endif

#endif	  //__APP_DBG_H__

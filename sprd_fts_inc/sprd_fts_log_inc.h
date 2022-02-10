#ifndef __SPRD_FTS_LOG_INC_H__
#define __SPRD_FTS_LOG_INC_H__

#ifdef SPRD_LOG_IOM
#include "log_api.h"

static int g_sprd_fts_logInit = 0;

#define printf(fmt, ...)               \
 do {                                       \
      if(g_sprd_fts_logInit == 0) { log_init(); g_sprd_fts_logInit = 1; } \
      info_log("%s: " fmt, SPRD_ENGPC_LOG_TAG, ##__VA_ARGS__); \
 } while (0)

#define __android_log_print(ANDROID_LOG_DEBUG, SPRD_ENGPC_LOG_TAG, fmt, ...)  \
 do {                                       \
      if(g_sprd_fts_logInit == 0) { log_init(); g_sprd_fts_logInit = 1; } \
      info_log("%s: " fmt, SPRD_ENGPC_LOG_TAG, ##__VA_ARGS__); \
 } while (0)

#define ALOGE(fmt, ...)               \
 do {                                       \
      if(g_sprd_fts_logInit == 0) { log_init(); g_sprd_fts_logInit = 1; } \
      err_log("%s: " fmt, SPRD_ENGPC_LOG_TAG, ##__VA_ARGS__); \
 } while (0)

#endif

#endif

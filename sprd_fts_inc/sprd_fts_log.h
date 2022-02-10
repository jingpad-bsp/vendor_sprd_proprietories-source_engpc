#ifndef __SPRD_FTS_LOG_H__
#define __SPRD_FTS_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <log/log.h>

#define SPRD_FTS_LOG_TAG "sprd_fts:"
#define SPRD_ENGPC_LOG_TAG "ENGPC:"

#include "sprd_fts_log_inc.h"

#define SPRD_FTS_TRACE

#ifdef SPRD_FTS_TRACE
#define SPRD_FTS_LOG(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, SPRD_FTS_LOG_TAG, fmt, ##args)
#define ENG_LOG(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, SPRD_ENGPC_LOG_TAG, fmt, ##args)
#else
#define SPRD_FTS_LOG(format, ...)
#define ENG_LOG(fmt, args...)
#endif

#define LOGI(fmt, args...) \
  __android_log_print(ANDROID_LOG_INFO, SPRD_FTS_LOG_TAG, fmt, ##args)
#define LOGD(fmt, args...) \
  __android_log_print(ANDROID_LOG_DEBUG, SPRD_FTS_LOG_TAG, fmt, ##args)
#define LOGE(fmt, args...) \
  __android_log_print(ANDROID_LOG_ERROR, SPRD_FTS_LOG_TAG, fmt, ##args)

#define FUN_ENTER             LOGD("[ %s ++ ]\n", __FUNCTION__)
#define FUN_EXIT              LOGD("[ %s -- ]\n", __FUNCTION__)

#ifdef __cplusplus
}
#endif

#endif

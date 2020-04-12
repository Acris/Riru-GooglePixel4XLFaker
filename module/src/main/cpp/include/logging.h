//
// Created by acris on 2020/4/12.
//

#ifndef RIRU_FAKEASPIXEL4XL_LOGGING_H
#define RIRU_FAKEASPIXEL4XL_LOGGING_H

#include "errno.h"
#include "android/log.h"

#ifndef LOG_TAG
#define LOG_TAG    "GooglePixel4XLFake"
#endif

#ifdef DEBUG
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif
#define LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define PLOGE(fmt, args...) LOGE(fmt " failed with %d: %s", ##args, errno, strerror(errno))

#endif //RIRU_FAKEASPIXEL4XL_LOGGING_H
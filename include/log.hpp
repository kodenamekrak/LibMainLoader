#pragma once

#include <android/log.h>

#define LOG_VERBOSE(...) __android_log_print(ANDROID_LOG_VERBOSE, "libmain - patched", __VA_ARGS__)
#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, "libmain - patched", __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, "libmain - patched", __VA_ARGS__)
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, "libmain - patched", __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, "libmain - patched", __VA_ARGS__)

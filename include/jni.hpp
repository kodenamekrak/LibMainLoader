#pragma once
#include <jni.h>

#include <string_view>

#include "_config.h"

namespace jni {

// For sv literal
using namespace std::string_view_literals;

constexpr auto unityName = "libunity.so"sv;
MAIN_LOCAL extern void* unityHandle;

using JNI_OnLoad_t = jint(JavaVM*, void*);
using JNI_OnUnload_t = jint(JavaVM*, void*);

MAIN_LOCAL jboolean load(JNIEnv* env, jobject klass, jstring str) noexcept;
MAIN_LOCAL jboolean unload(JNIEnv* env, jobject klass) noexcept;

}  // namespace jni

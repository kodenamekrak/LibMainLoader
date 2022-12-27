#pragma once
#include <jni.h>
#include <string_view>

namespace jni {

    constexpr const std::string_view unityName = "libunity.so";
    extern void* unityHandle;

    using JNI_OnLoad_t = jint(JavaVM*, void*);
    using JNI_OnUnload_t = jint(JavaVM*, void*);

    jboolean load(JNIEnv* env, jobject klass, jstring str) noexcept;
    jboolean unload(JNIEnv* env, jobject klass) noexcept;

}
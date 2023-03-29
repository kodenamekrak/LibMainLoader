#pragma once
#include <jni.h>
#include <string_view>

namespace jni {

    // For sv literal
    using namespace std::string_view_literals;

    constexpr auto unityName = "libunity.so"sv;
    extern void* unityHandle;

    using JNI_OnLoad_t = jint(JavaVM*, void*);
    using JNI_OnUnload_t = jint(JavaVM*, void*);

    jboolean load(JNIEnv* env, jobject klass, jstring str) noexcept;
    jboolean unload(JNIEnv* env, jobject klass) noexcept;

}
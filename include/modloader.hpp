#pragma once

#include <jni.h>
#include <string_view>
#include <filesystem>

namespace modloader {

    constexpr const std::string_view modloaderName = "libmodloader.so";
    extern void* modloaderHandle;
    
    /*Called after modloader gets dlopened in JNI_OnLoad*/
    using preload_t = void(JNIEnv* env, const char* path) noexcept;
    /*Called before unity gets dlopened*/
    using load_t = void(JNIEnv* env) noexcept;
    /*Called after unity gets dlopened*/
    using accept_unity_handle_t = void(JNIEnv* env, void* unityHandle) noexcept;
    using unload_t = void(JNIEnv* env) noexcept;

    void preload(JNIEnv* env) noexcept;
    void load(JNIEnv* env) noexcept;
    void accept_unity_handle(JNIEnv* env, void* unityHandle) noexcept;
    void unload(JNIEnv* env) noexcept;

}
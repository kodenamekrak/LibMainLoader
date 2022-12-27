#pragma once

#include <jni.h>
#include <string_view>
#include <filesystem>

namespace modloader {

    constexpr const std::string_view modloaderName = "libmodloader.so";
    extern void* modloaderHandle;
    
    /*Called when modloader gets dlopened in JNI_OnLoad*/
    using preload_t = void(JNIEnv* env, std::filesystem::path path) noexcept;
    /*Called before unity gets dlopened*/
    using load_t = void(JNIEnv* env) noexcept;
    /*Called after unity gets dlopened*/
    using init_t = void(JNIEnv* env) noexcept;
    using unload_t = void(JNIEnv* env) noexcept;

    void preload(JNIEnv* env) noexcept;
    void load(JNIEnv* env) noexcept;
    void init(JNIEnv* env) noexcept;
    void unload(JNIEnv* env) noexcept;

}
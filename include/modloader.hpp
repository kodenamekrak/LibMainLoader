#pragma once

#include <jni.h>
#include <string_view>
#include <filesystem>

namespace modloader {

    // For sv literal
    using namespace std::string_view_literals;

    constexpr auto modloaderName = "libmodloader.so"sv;
    extern void* modloaderHandle;
    
    /// @brief Called after modloader gets dlopened in JNI_OnLoad.
    /// The lifetimes of pointers are within this call only and should be copied.
    using preload_t = void(JNIEnv* env, char const* modloaderPath, char const* filesDir, char const* externalDir) noexcept;
    constexpr auto preloadName = "modloader_preload"sv;
    /// @brief Called before unity gets dlopened
    using load_t = void(JNIEnv* env) noexcept;
    constexpr auto loadName = "modloader_load"sv;
    /// @brief Called after unity gets dlopened
    using accept_unity_handle_t = void(JNIEnv* env, void* unityHandle) noexcept;
    constexpr auto accept_unity_handleName = "modloader_accept_unity_handle"sv;
    /// @brief Called during teardown (sometimes)
    using unload_t = void(JNIEnv* env) noexcept;
    constexpr auto unloadName = "modloader_unload"sv;

    void preload(JNIEnv* env) noexcept;
    void load(JNIEnv* env) noexcept;
    void accept_unity_handle(JNIEnv* env, void* unityHandle) noexcept;
    void unload(JNIEnv* env) noexcept;

}
#pragma once

#include <jni.h>

#include <filesystem>
#include <string_view>

#include "_config.h"

namespace modloader {

// For sv literal
using namespace std::string_view_literals;

MAIN_LOCAL extern void* modloaderHandle;

/// @brief Called after modloader gets dlopened in JNI_OnLoad.
/// The lifetimes of pointers are within this call only and should be copied.
using preload_t = void(JNIEnv* env, char const* appId, char const* modloaderPath, char const* modloaderSourcePath,
                       char const* filesDir, char const* externalDir) noexcept;
MAIN_LOCAL constexpr auto preloadName = "modloader_preload"sv;
/// @brief Called before unity gets dlopened
using load_t = void(JNIEnv* env, char const* soDir) noexcept;
MAIN_LOCAL constexpr auto loadName = "modloader_load"sv;
/// @brief Called after unity gets dlopened
using accept_unity_handle_t = void(JNIEnv* env, void* unityHandle) noexcept;
MAIN_LOCAL constexpr auto accept_unity_handleName = "modloader_accept_unity_handle"sv;
/// @brief Called during teardown (sometimes)
using unload_t = void(JavaVM* vm) noexcept;
MAIN_LOCAL constexpr auto unloadName = "modloader_unload"sv;

MAIN_LOCAL void preload(JNIEnv* env) noexcept;
MAIN_LOCAL void load(JNIEnv* env, char const* soDir) noexcept;
MAIN_LOCAL void accept_unity_handle(JNIEnv* env, void* unityHandle) noexcept;
MAIN_LOCAL void unload(JavaVM* vm) noexcept;

}  // namespace modloader

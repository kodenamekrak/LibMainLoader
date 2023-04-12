#pragma once

#include "_config.h"
#include <jni.h>
#include <string>
#include <optional>
#include <filesystem>

namespace fileutils {

    struct MAIN_LOCAL path_container {
        std::filesystem::path filesDir;
        std::filesystem::path externalDir;
    };

    MAIN_LOCAL std::optional<path_container> getDirs(JNIEnv* env);
    MAIN_LOCAL std::optional<std::string> getApplicationId();
    MAIN_LOCAL bool ensurePerms(JNIEnv* env, jobject activity);
    MAIN_LOCAL jobject getActivityFromUnityPlayer(JNIEnv *env);

}
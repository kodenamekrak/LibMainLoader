#pragma once

#include <jni.h>
#include <string>
#include <optional>
#include <filesystem>

namespace fileutils {

    struct path_container {
        std::filesystem::path filesDir;
        std::filesystem::path externalDir;
    };

    std::optional<path_container> getDirs(JNIEnv* env);
    std::optional<std::string> getApplicationId();

}
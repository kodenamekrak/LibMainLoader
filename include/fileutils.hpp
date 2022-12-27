#pragma once

#include <jni.h>
#include <string>
#include <optional>
#include <filesystem>

namespace fileutils {

    std::optional<std::filesystem::path> getFilesDir(JNIEnv* env);
    std::optional<std::string> getApplicationId();

}
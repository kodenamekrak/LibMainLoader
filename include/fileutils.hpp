#pragma once

#include <jni.h>

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>

#include "_config.h"

namespace fileutils {

struct MAIN_LOCAL path_container {
  std::filesystem::path modloaderSearchPath;
  std::filesystem::path filesDir;
  std::filesystem::path externalDir;
};

MAIN_LOCAL std::optional<path_container> getDirs(JNIEnv* env, std::string_view application_id);
MAIN_LOCAL std::optional<std::string> getApplicationId();
MAIN_LOCAL bool ensurePerms(JNIEnv* env, jobject activity, std::string_view application_id);
MAIN_LOCAL jobject getActivityFromUnityPlayer(JNIEnv* env);

namespace literals {

// To facilitate easier combination
MAIN_LOCAL inline std::filesystem::path operator""_fp(const char* p, [[maybe_unused]] std::size_t sz) {
  return std::filesystem::path(p);
}

}  // namespace literals

}  // namespace fileutils

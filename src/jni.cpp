#include "jni.hpp"

#include <dlfcn.h>

#include <array>
#include <filesystem>
#include <string>

#include "log.hpp"
#include "modloader.hpp"

namespace fs = std::filesystem;

MAIN_LOCAL void* jni::unityHandle = nullptr;

MAIN_LOCAL jboolean jni::load(JNIEnv* env, [[maybe_unused]] jobject klass, jstring str) noexcept {
  if (unityHandle != nullptr) return true;

  auto chars = env->GetStringUTFChars(str, nullptr);
  fs::path path(chars);
  env->ReleaseStringUTFChars(str, chars);

  LOG_VERBOSE("Searching in %s", path.c_str());

  JavaVM* vm = nullptr;

  if (env->GetJavaVM(&vm) < 0) {
    env->FatalError("Unable to retrieve Java VM");  // libmain wording
    return false;                                   // doesn't actually reach here
  }

  LOG_VERBOSE("Got JVM");

  modloader::load(env, path.c_str());

  path /= unityName;
  unityHandle = dlopen(path.c_str(), RTLD_LAZY);
  if (unityHandle == nullptr) {
    unityHandle = dlopen(unityName.data(), RTLD_LAZY);
    if (unityHandle == nullptr) {
      auto err = dlerror();
      LOG_ERROR("Could not load unity from %s: %s", path.c_str(), err);
      std::array<char, 0x400> message = {0};  // this is what the original libmain allocates, so lets hope its enough
                                              // and doesn't use all of the rest of our stack space
      snprintf(message.data(), message.size(), "Unable to load library: %s [%s]", path.c_str(), err);
      env->FatalError(message.data());
      return false;  // doesn't actually reach here
    }
  }

  modloader::accept_unity_handle(env, unityHandle);

  auto onload = reinterpret_cast<JNI_OnLoad_t*>(dlsym(unityHandle, "JNI_OnLoad"));
  if (onload != nullptr) {
    if (onload(vm, nullptr) > JNI_VERSION_1_6) {
      LOG_ERROR("unity JNI_OnLoad requested unsupported VM version");
      env->FatalError("Unsupported VM version");  // libmain wording
      return false;                               // doesn't actually reach here
    }
  } else {
    LOG_WARN("unity does not have a JNI_OnLoad");
  }

  LOG_INFO("Successfully loaded and initialized %s", path.c_str());

  return unityHandle != nullptr;
}

MAIN_LOCAL jboolean jni::unload(JNIEnv* env, [[maybe_unused]] jobject klass) noexcept {
  JavaVM* vm = nullptr;

  if (env->GetJavaVM(&vm) < 0) {
    env->FatalError("Unable to retrieve Java VM");  // libmain wording
    return false;                                   // doesn't actually reach here
  }

  modloader::unload(vm);

  auto onunload = reinterpret_cast<JNI_OnUnload_t*>(dlsym(unityHandle, "JNI_OnUnload"));
  if (onunload != nullptr) {
    onunload(vm, nullptr);
  } else {
    LOG_WARN("unity does not have a JNI_OnUnload");
  }

  int code = dlclose(unityHandle);

  if (code != 0) {
    LOG_ERROR("Error occurred closing unity: %s", dlerror());
  } else {
    LOG_VERBOSE("Successfully closed unity");
  }
  return true;
}

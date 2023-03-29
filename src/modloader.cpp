#include "log.hpp"
#include "modloader.hpp"
#include "fileutils.hpp"

#include <dlfcn.h>
#include <system_error>

namespace fs = std::filesystem;

void* modloader::modloaderHandle = nullptr;

void modloader::preload(JNIEnv* env) noexcept {
    LOG_VERBOSE("preload: Attempting to load modloader");

    auto applicationId = fileutils::getApplicationId();
    if(!applicationId) {
        LOG_WARN("preload: Failed to get ApplicationId");
        return;
    }
    auto pathContainer = fileutils::getDirs(env);
    if(!pathContainer) {
        LOG_WARN("preload: Failed to get path container");
        return;
    }

    fs::path src(pathContainer->externalDir / modloaderName);
    fs::path modloaderPath(pathContainer->filesDir / modloaderName);

    std::error_code error_code;
    if (!fs::exists(src, error_code)) {
        LOG_WARN("preload: Failed to find modloader at %s (error: %s)", src.c_str(), error_code.message().c_str());
        return;
    }
    if (!fs::copy_file(src, modloaderPath, fs::copy_options::overwrite_existing, error_code)) {
        LOG_WARN("preload: Failed to copy %s to %s (error: %s)", src.c_str(), modloaderPath.c_str(), error_code.message().c_str());
        return;
    }
    
    modloaderHandle = dlopen(modloaderPath.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (modloaderHandle == nullptr) {
        LOG_WARN("preload: Could not load %s: %s", modloaderPath.c_str(), dlerror());
        return;
    }

    LOG_VERBOSE("preload: Loaded modloader: %s", modloaderPath.c_str());

    auto preload = reinterpret_cast<preload_t*>(dlsym(modloaderHandle, preloadName.data()));
    if (preload == nullptr) {
        LOG_WARN("preload: %s does not have %s: %s", preloadName.data(), modloaderPath.c_str(), dlerror());
        return;
    }

    LOG_VERBOSE("preload: Calling %s", preloadName.data());
    preload(env, modloaderPath.c_str(), pathContainer->filesDir.c_str(), pathContainer->externalDir.c_str());

    LOG_VERBOSE("preload: Preloading done");
}

void modloader::load(JNIEnv* env) noexcept { 
    if (modloaderHandle == nullptr) {
        LOG_WARN("load: Modloader not loaded!");
        return;
    }
    auto load = reinterpret_cast<load_t*>(dlsym(modloaderHandle, loadName.data()));
    if (load == nullptr) {
        LOG_WARN("load: Failed to find %s: %s", loadName.data(), dlerror());
        return;
    }
    LOG_VERBOSE("load: Calling %s", loadName.data());
    load(env);
    LOG_VERBOSE("load: Loading done");
}

void modloader::accept_unity_handle(JNIEnv* env, void* unityHandle) noexcept {
    if (modloaderHandle == nullptr) {
        LOG_WARN("accept_unity_handle: Modloader not loaded!");
        return;
    }
    auto accept_unity_handle = reinterpret_cast<accept_unity_handle_t*>(dlsym(modloaderHandle, accept_unity_handleName.data()));
    if (accept_unity_handle == nullptr) {
        LOG_WARN("accept_unity_handle: Failed to find %s: %s", accept_unity_handleName.data(), dlerror());
        return;
    }
    LOG_VERBOSE("accept_unity_handle: Calling %s", accept_unity_handleName.data());
    accept_unity_handle(env, unityHandle);
    LOG_VERBOSE("accept_unity_handle: Init done");
}

void modloader::unload(JNIEnv* env) noexcept {
    if (modloaderHandle == nullptr) {
        LOG_WARN("unload: Modloader not loaded!");
        return;
    }
    auto unload = reinterpret_cast<unload_t*>(dlsym(modloaderHandle, unloadName.data()));
    if (unload == nullptr) {
        LOG_WARN("unload: Failed to find %s: %s", unloadName.data(), dlerror());
        return;
    }
    LOG_VERBOSE("unload: Calling %s", unloadName.data());
    unload(env);
    LOG_VERBOSE("unload: Unload done");
}
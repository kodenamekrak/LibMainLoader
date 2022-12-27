#include "log.hpp"
#include "modloader.hpp"
#include "fileutils.hpp"

#include <dlfcn.h>

namespace fs = std::filesystem;

void* modloader::modloaderHandle = nullptr;

void modloader::preload(JNIEnv* env) noexcept {
    LOG_VERBOSE("Attempting to load modloader in modloader::preload()");

    auto applicationId = fileutils::getApplicationId();
    if(!applicationId) {
        LOG_WARN("Could get ApplicationId");
        return;
    }
    auto filesDir = fileutils::getFilesDir(env);
    if(!filesDir) {
        LOG_WARN("Could get FilesDir");
        return;
    }

    fs::path src = fs::path("/sdcard/Android/data") / applicationId.value() / "files" / modloaderName.data();
    fs::path modloaderPath = filesDir.value() / modloaderName.data();

    if (!fs::exists(src)) {
        LOG_WARN("Could find modloader at %s", src.c_str());
        return;
    }
    try {
        if(!fs::copy_file(src, modloaderPath, fs::copy_options::overwrite_existing)) {
            LOG_WARN("Could copy %s to %s", src.c_str(), modloaderPath.c_str());
            return;
        }
    } catch(fs::filesystem_error& e) {
        LOG_WARN("Could copy %s to %s: %s", src.c_str(), modloaderPath.c_str(), e.what());
        return;
    }
    
    modloaderHandle = dlopen(modloaderPath.c_str(), RTLD_LAZY);
    if (modloaderHandle == nullptr) {
        LOG_WARN("Could not load %s: %s", modloaderPath.c_str(), dlerror());
        return;
    }

    LOG_VERBOSE("%s loaded", modloaderPath.c_str());

    auto preload = reinterpret_cast<preload_t*>(dlsym(modloaderHandle, "modloader_preload"));
    if (preload == nullptr) {
        LOG_WARN("%s does not have modloader_preload: %s", modloaderPath.c_str(), dlerror());
        return;
    }

    LOG_VERBOSE("Calling modloader_preload");
    preload(env, modloaderPath.c_str());

    LOG_VERBOSE("Preloading done");
}

void modloader::load(JNIEnv* env) noexcept { 
    if (modloaderHandle == nullptr) {
        LOG_WARN("Modloader not loaded!");
        return;
    }
    auto load = reinterpret_cast<load_t*>(dlsym(modloaderHandle, "modloader_load"));
    if (load == nullptr) {
        LOG_WARN("Couldn't find modloader_load: %s", dlerror());
        return;
    }
    LOG_VERBOSE("Calling modloader_load");
    load(env);
    LOG_VERBOSE("Loading done");
}

void modloader::init(JNIEnv* env) noexcept {
    if (modloaderHandle == nullptr) {
        LOG_WARN("Modloader not loaded!");
        return;
    }
    auto init = reinterpret_cast<init_t*>(dlsym(modloaderHandle, "modloader_init"));
    if (init == nullptr) {
        LOG_WARN("Couldn't find modloader_init: %s", dlerror());
        return;
    }
    LOG_VERBOSE("Calling modloader_init");
    init(env);
    LOG_VERBOSE("Init done");
}

void modloader::unload(JNIEnv* env) noexcept {
    if (modloaderHandle == nullptr) {
        LOG_WARN("Modloader not loaded!");
        return;
    }
    auto unload = reinterpret_cast<unload_t*>(dlsym(modloaderHandle, "modloader_unload"));
    if (unload == nullptr) {
        LOG_WARN("Couldn't find modloader_unload: %s", dlerror());
        return;
    }
    LOG_VERBOSE("Calling modloader_unload");
    unload(env);
    LOG_VERBOSE("Unload done");
}
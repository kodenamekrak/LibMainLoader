#include <array>

#include "log.hpp"
#include "jni.hpp"
#include "modloader.hpp"
#include "fileutils.hpp"

#define EXPORT_FUNC extern "C" __attribute__((visibility("default")))

constexpr std::array<JNINativeMethod, 2> NativeLoader_bindings = {{
    { "load",   "(Ljava/lang/String;)Z", (void*)&jni::load   },
    { "unload", "()Z",                   (void*)&jni::unload }
}};

EXPORT_FUNC jint JNI_OnLoad(JavaVM* vm, void*) {
    JNIEnv* env = nullptr;

    LOG_INFO("JNI_OnLoad called, linking JNI methods");

    vm->AttachCurrentThread(&env, nullptr);
    auto klass = env->FindClass("com/unity3d/player/NativeLoader");

    auto ret = env->RegisterNatives(klass, NativeLoader_bindings.data(), NativeLoader_bindings.size());

    if (ret < 0) {
        LOG_ERROR("RegisterNatives failed with %d", ret);

        env->FatalError("com/unity3d/player/NativeLoader"); // this is such a useless fucking error message because the original libmain does this

        return -1;
    }

    LOG_VERBOSE("Calling modloader preload");
    modloader::preload(env);
    
    LOG_INFO("JNI_OnLoad done!");

    return JNI_VERSION_1_6;
}

EXPORT_FUNC void JNI_OnUnload(JavaVM* vm, void*) {
    LOG_INFO("JNI_OnUnload called!");
}
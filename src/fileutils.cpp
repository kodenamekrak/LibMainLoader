#include "log.hpp"
#include "fileutils.hpp"

#include <jni.h>
#include <ctype.h>
#include <cstring>

namespace fs = std::filesystem;

namespace {
char *trimWhitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

jobject getActivityFromUnityPlayerInternal(JNIEnv *env) {
    jclass clazz = env->FindClass("com/unity3d/player/UnityPlayer");
    if (clazz == NULL) return nullptr;
    jfieldID actField = env->GetStaticFieldID(clazz, "currentActivity", "Landroid/app/Activity;");
    if (actField == NULL) return nullptr;
    return env->GetStaticObjectField(clazz, actField);
}

bool ensurePermsInternal(JNIEnv* env, jobject activity) {
    // We only need the read permission here, since we need to copy over the modloader so we can open it. 
    // That said, we should request for MANAGE because it gives us a bit more power (and the modloader will want it anyways)
    jclass clazz = env->FindClass("com/unity3d/player/UnityPlayerActivity");
    if (clazz == nullptr) return false;
    jmethodID checkSelfPermission = env->GetMethodID(clazz, "checkSelfPermission", "(Ljava/lang/String;)I");
    if (checkSelfPermission == nullptr) return false;
    const jstring perm = env->NewStringUTF("android.permission.MANAGE_EXTERNAL_STORAGE");
    jint hasPerm = env->CallIntMethod(activity, checkSelfPermission, perm);
    LOG_DEBUG("checkSelfPermission(MANAGE_EXTERNAL_STORAGE) returned: %i", hasPerm);
    if (hasPerm != 0) {
        jmethodID requestPermissions = env->GetMethodID(clazz, "requestPermissions", "([Ljava/lang/String;I)V");
        if (requestPermissions == nullptr) return false;
        jclass stringClass = env->FindClass("java/lang/String");
        jobjectArray arr = env->NewObjectArray(1, stringClass, perm);
        constexpr jint requestCode = 21326; // arbitrary
        LOG_INFO("Calling requestPermissions!");
        env->CallVoidMethod(activity, requestPermissions, arr, requestCode);
        if (env->ExceptionCheck()) return false;
    }
    return true;
}

}

#define NULLOPT_UNLESS(expr, ...) ({ \
auto&& __tmp = (expr); \
if (!__tmp) {LOG_ERROR(__VA_ARGS__); return std::nullopt;} \
__tmp; })

std::optional<fileutils::path_container> fileutils::getDirs(JNIEnv* env) {
    auto activityThreadClass = NULLOPT_UNLESS(env->FindClass("android/app/ActivityThread"), "Failed to find android.app.ActivityThread!");
    auto currentActivityThreadMethod = NULLOPT_UNLESS(env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;"), "Failed to find ActivityThread.currentActivityThread");
    auto contextClass = NULLOPT_UNLESS(env->FindClass("android/content/Context"), "Failed to find android.context.Context!");
    auto getApplicationMethod = NULLOPT_UNLESS(env->GetMethodID(activityThreadClass, "getApplication", "()Landroid/app/Application;"), "Failed to find Application.getApplication");
    auto filesDirMethod = NULLOPT_UNLESS(env->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;"), "Failed to find Context.getFilesDir()");
    auto externalDirMethod = NULLOPT_UNLESS(env->GetMethodID(contextClass, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;"), "Failed to find Context.getExternalFilesDir(String)");
    auto fileClass = NULLOPT_UNLESS(env->FindClass("java/io/File"), "Failed to find java.io.File!");
    auto absDirMethod = NULLOPT_UNLESS(env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;"), "Failed to find File.getAbsolutePath()");

    fileutils::path_container result{};

    auto at = NULLOPT_UNLESS(env->CallStaticObjectMethod(activityThreadClass, currentActivityThreadMethod), "Returned result from currentActivityThread is null!");
    auto context = NULLOPT_UNLESS(env->CallObjectMethod(at, getApplicationMethod), "Returned result from getApplication is null!");
    auto filesDir = NULLOPT_UNLESS(env->CallObjectMethod(context, filesDirMethod), "Returned result from getFilesDir is null!");
    {
        auto str = reinterpret_cast<jstring>(NULLOPT_UNLESS(env->CallObjectMethod(filesDir, absDirMethod), "Returned result from getAbsolutePath is null!"));
        auto chars = env->GetStringUTFChars(str, nullptr);
        result.filesDir = chars;
        env->ReleaseStringUTFChars(str, chars);
    }

    auto externalDir = NULLOPT_UNLESS(env->CallObjectMethod(context, externalDirMethod, static_cast<jstring>(nullptr)), "Returned result from getExternalFilesDir is null!");
    {
        auto str = reinterpret_cast<jstring>(NULLOPT_UNLESS(env->CallObjectMethod(externalDir, absDirMethod), "Returned result from getAbsolutePath is null!"));
        auto chars = env->GetStringUTFChars(str, nullptr);
        result.externalDir = chars;
        env->ReleaseStringUTFChars(str, chars);
    }
    return result;
}

std::optional<std::string> fileutils::getApplicationId() {
    FILE *cmdline = fopen("/proc/self/cmdline", "r");
    if (cmdline) {
        //not sure what the actual max is, but path_max should cover it
        char application_id[PATH_MAX] = {0};
        fread(application_id, sizeof(application_id), 1, cmdline);
        fclose(cmdline);
        trimWhitespace(application_id);
        return std::string(application_id);
    }
    return std::nullopt;
}

jobject fileutils::getActivityFromUnityPlayer(JNIEnv *env) {
    jobject activity = getActivityFromUnityPlayerInternal(env);
    if (activity == nullptr) {
        if (env->ExceptionCheck()) env->ExceptionDescribe();
        LOG_ERROR("libmain.getActivityFromUnityPlayer failed! See 'System.err' tag.");
        env->ExceptionClear();
    }
    return activity;
}

bool fileutils::ensurePerms(JNIEnv* env, jobject activity) {
    if (ensurePermsInternal(env, activity)) return true;

    if (env->ExceptionCheck()) env->ExceptionDescribe();
    LOG_ERROR("libmain.ensurePerms failed! See 'System.err' tag.");
    env->ExceptionClear();
    return false;
}

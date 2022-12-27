#include "log.hpp"
#include "fileutils.hpp"

#include <jni.h>
#include <ctype.h>

namespace fs = std::filesystem;

#define NULLOPT_UNLESS(expr, ...) ({ \
auto&& __tmp = (expr); \
if (!__tmp) {LOG_ERROR(__VA_ARGS__); return std::nullopt;} \
__tmp; })

std::optional<fs::path> fileutils::getFilesDir(JNIEnv* env) {
    auto activityThreadClass = NULLOPT_UNLESS(env->FindClass("android/app/ActivityThread"), "Failed to find android.app.ActivityThread!");
    auto currentActivityThreadMethod = NULLOPT_UNLESS(env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;"), "Failed to find ActivityThread.currentActivityThread");
    auto contextClass = NULLOPT_UNLESS(env->FindClass("android/content/Context"), "Failed to find android.context.Context!");
    auto getApplicationMethod = NULLOPT_UNLESS(env->GetMethodID(activityThreadClass, "getApplication", "()Landroid/app/Application;"), "Failed to find Application.getApplication");
    auto filesDirMethod = NULLOPT_UNLESS(env->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;"), "Failed to find Context.getFilesDir()!");
    auto fileClass = NULLOPT_UNLESS(env->FindClass("java/io/File"), "Failed to find java.io.File!");
    auto absDirMethod = NULLOPT_UNLESS(env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;"), "Failed to find File.getAbsolutePath()!");

    auto at = NULLOPT_UNLESS(env->CallStaticObjectMethod(activityThreadClass, currentActivityThreadMethod), "Returned result from currentActivityThread is null!");
    auto context = NULLOPT_UNLESS(env->CallObjectMethod(at, getApplicationMethod), "Returned result from getApplication is null!");
    auto file = NULLOPT_UNLESS(env->CallObjectMethod(context, filesDirMethod), "Returned result from getFilesDir is null!");
    auto str = reinterpret_cast<jstring>(NULLOPT_UNLESS(env->CallObjectMethod(file, absDirMethod), "Returned result from getAbsolutePath is null!"));
    
    auto chars = env->GetStringUTFChars(str, nullptr);
    fs::path path(chars);
    env->ReleaseStringUTFChars(str, chars);
    return path;
}

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

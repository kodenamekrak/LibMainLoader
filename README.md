# LibMainLoader

Acts as a proxy `libmain.so` for Unity code injection via il2cpp on Android.

Specifically stubs out: `JNI_OnLoad`, `JNI_OnUnload`

## Building

1. Make `ndkpath.txt` point to your NDK install of at least r25.
1. Run `build.ps1`, which will configure CMake for you, and make a `build` directory, then will execute `ninja -C build` on that directory.

To adjust any build flags, edit `CMakeLists.txt` directly, or forward the args through the command line.

To build with decreased codesize (although not substantially), define `NO_EXCESS_LOGGING`.

## Usage

After building, which targets AArch64, replace the existing `libmain.so` in your unity application with the built `libmain.so`.

Then, place a [modloader](#modloader) at: `/sdcard/ModData/<application id>/Modloader/*.so`.

This binary will walk the

## Modloader

Ultimately a modloader must be `dlopen`-able without any specific dependencies. It should also be okay with being dlopened into the `GLOBAL` symbol lookup table. After that, it may _optionally_ define the following (`extern C`) functions, which will be called at various points during the loading process:

| Signature | Call point |
| --------- | ---------- |
| `void modloader_preload(JNIEnv* env, char const* appId, char const* modloaderPath, char const* modloaderSourcePath, char const* filesDir, char const* externalDir) noexcept` | Called immediately after dlopen of the modloader. Forwards parameters that may be useful (largely to avoid duplicate JNI calls). |
| `void modloader_load(JNIEnv* env, char const* soDir) noexcept` | Called after the `JavaVM` is found as part of the main loading process, via `JNI_OnLoad`. |
| `void modloader_accept_unity_handle(JNIEnv* env, void* unityHandle) noexcept` | Called after `libunity.so` is dlopened, but before Unity's `JNI_OnLoad` is called. |
| `void modloader_unload(JavaVM* vm) noexcept` | Called when `JNI_OnUnload` is called, after the `JavaVM` is obtained during teardown. |

## Resources

Updated libmain part from <https://github.com/sc2ad/QuestLoader>

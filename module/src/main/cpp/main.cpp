#include <jni.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <unistd.h>
#include "include/logging.h"
#include "include/hook.h"

static char package_name[256];
static int uid;
static int enable_hook;
static std::vector<std::string> globalPkgBlackList = {"com.oneplus.camera", "com.oneplus.gallery"};


bool isAppNeedHook(JNIEnv *env, jstring jAppDataDir, jstring jPackageName) {
    if (jPackageName) {
        const char *packageName = env->GetStringUTFChars(jPackageName, nullptr);
        sprintf(package_name, "%s", packageName);
        env->ReleaseStringUTFChars(jPackageName, packageName);
    } else if (jAppDataDir) {
        const char *appDataDir = env->GetStringUTFChars(jAppDataDir, nullptr);
        int user = 0;
        while (true) {
            // /data/user/<user_id>/<package>
            if (sscanf(appDataDir, "/data/%*[^/]/%d/%s", &user, package_name) == 2)
                break;

            // /mnt/expand/<id>/user/<user_id>/<package>
            if (sscanf(appDataDir, "/mnt/expand/%*[^/]/%*[^/]/%d/%s", &user, package_name) == 2)
                break;

            // /data/data/<package>
            if (sscanf(appDataDir, "/data/%*[^/]/%s", package_name) == 1)
                break;

            package_name[0] = '\0';
            return false;
        }
        env->ReleaseStringUTFChars(jAppDataDir, appDataDir);
    } else {
        return false;
    }

    std::string pkgName = package_name;
    for (auto &s : globalPkgBlackList) {
        if (pkgName.find(s) != std::string::npos) {
            return false;
        }
    }

    return true;
}

void injectBuild(JNIEnv *env) {
    if (env == nullptr) {
        LOGW("failed to inject android.os.Build for %s due to env is null", package_name);
        return;
    }
    LOGI("inject android.os.Build for %s ", package_name);

    jclass build_class = env->FindClass("android/os/Build");
    if (build_class == nullptr) {
        LOGW("failed to inject android.os.Build for %s due to build is null", package_name);
        return;
    }

    jstring brand = env->NewStringUTF("google");
    jstring manufacturer = env->NewStringUTF("Google");
    jstring product = env->NewStringUTF("coral");
    jstring model = env->NewStringUTF("Pixel 4 XL");

    jfieldID brand_id = env->GetStaticFieldID(build_class, "BRAND", "Ljava/lang/String;");
    if (brand_id != nullptr) {
        env->SetStaticObjectField(build_class, brand_id, brand);
    }

    jfieldID manufacturer_id = env->GetStaticFieldID(build_class, "MANUFACTURER", "Ljava/lang/String;");
    if (manufacturer_id != nullptr) {
        env->SetStaticObjectField(build_class, manufacturer_id, manufacturer);
    }

    jfieldID product_id = env->GetStaticFieldID(build_class, "PRODUCT", "Ljava/lang/String;");
    if (product_id != nullptr) {
        env->SetStaticObjectField(build_class, product_id, product);
    }

    jfieldID device_id = env->GetStaticFieldID(build_class, "DEVICE", "Ljava/lang/String;");
    if (device_id != nullptr) {
        env->SetStaticObjectField(build_class, device_id, product);
    }

    jfieldID model_id = env->GetStaticFieldID(build_class, "MODEL", "Ljava/lang/String;");
    if (model_id != nullptr) {
        env->SetStaticObjectField(build_class, model_id, model);
    }

    if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }

    env->DeleteLocalRef(brand);
    env->DeleteLocalRef(manufacturer);
    env->DeleteLocalRef(product);
    env->DeleteLocalRef(model);
}

static void appProcessPre(JNIEnv *env, jint _uid, jstring appDataDir, jstring packageName) {
    uid = _uid;
    enable_hook = isAppNeedHook(env, appDataDir, packageName);
}


static void appProcessPost(JNIEnv *env) {
    if (enable_hook) {
        injectBuild(env);
        install_hook(package_name, uid / 100000);
    }
}

extern "C" {
#define EXPORT __attribute__((visibility("default"))) __attribute__((used))
EXPORT void nativeForkAndSpecializePre(
        JNIEnv *env, jclass clazz, jint *_uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jintArray *fdsToClose, jintArray *fdsToIgnore, jboolean *is_child_zygote,
        jstring *instructionSet, jstring *appDataDir, jstring *packageName,
        jobjectArray *packagesForUID, jstring *sandboxId) {
    // packageName, packagesForUID, sandboxId are added from Android Q beta 2, removed from beta 5
    appProcessPre(env, *_uid, *appDataDir, *packageName);
}

EXPORT int nativeForkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    if (res != 0) return 0;
    appProcessPost(env);
    return 0;
}

EXPORT __attribute__((visibility("default"))) void specializeAppProcessPre(
        JNIEnv *env, jclass clazz, jint *_uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jboolean *startChildZygote, jstring *instructionSet, jstring *appDataDir,
        jstring *packageName, jobjectArray *packagesForUID, jstring *sandboxId) {
    // this is added from Android Q beta, but seems Google disabled this in following updates

    // packageName, packagesForUID, sandboxId are added from Android Q beta 2, removed from beta 5
    appProcessPost(env);
}

EXPORT __attribute__((visibility("default"))) int specializeAppProcessPost(
        JNIEnv *env, jclass clazz) {
    // this is added from Android Q beta, but seems Google disabled this in following updates
    appProcessPost(env);
    return 0;
}
}
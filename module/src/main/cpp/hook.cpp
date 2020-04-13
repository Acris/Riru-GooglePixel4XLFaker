//
// Created by acris on 2020/4/12.
//
#include <cstdio>
#include <cstring>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>
#include <cstdlib>
#include <string>
#include <sys/system_properties.h>

#include "external/libxhook/include/xhook.h"
#include "include/riru.h"
#include "include/logging.h"

#define XHOOK_REGISTER(NAME) \
    if (xhook_register(".*", #NAME, (void*) new_##NAME, (void **) &old_##NAME) == 0) { \
        if (riru_get_version() >= 8) { \
            void *f = riru_get_func(#NAME); \
            if (f != nullptr) \
                memcpy(&old_##NAME, &f, sizeof(void *)); \
            riru_set_func(#NAME, (void *) new_##NAME); \
        } \
    } else { \
        LOGE("failed to register hook " #NAME "."); \
    }

#define NEW_FUNC_DEF(ret, func, ...) \
    static ret (*old_##func)(__VA_ARGS__); \
    static ret new_##func(__VA_ARGS__)

NEW_FUNC_DEF(int, __system_property_get, const char *key, char *value) {
    int res = old___system_property_get(key, value);
    if (key) {
        if (strcmp("ro.product.system.brand", key) == 0) {
            strcpy(value, "google");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.system.device", key) == 0) {
            strcpy(value, "generic");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.system.manufacturer", key) == 0) {
            strcpy(value, "Google");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.system.model", key) == 0) {
            strcpy(value, "mainline");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.system.name", key) == 0) {
            strcpy(value, "mainline");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.vendor.brand", key) == 0) {
            strcpy(value, "google");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.vendor.device", key) == 0) {
            strcpy(value, "coral");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.vendor.manufacturer", key) == 0) {
            strcpy(value, "Google");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.vendor.model", key) == 0) {
            strcpy(value, "Pixel 4 XL");
            LOGI("system_property_get: %s -> %s", key, value);
        } else if (strcmp("ro.product.vendor.name", key) == 0) {
            strcpy(value, "coral");
            LOGI("system_property_get: %s -> %s", key, value);
        }
    }
    return res;
}

NEW_FUNC_DEF(std::string, _ZN7android4base11GetPropertyERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEES9_, const std::string &key, const std::string &default_value) {
    std::string res = old__ZN7android4base11GetPropertyERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEES9_(key, default_value);
    if (strcmp("ro.product.system.brand", key.c_str()) == 0) {
        res = "google";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.system.device", key.c_str()) == 0) {
        res = "generic";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.system.manufacturer", key.c_str()) == 0) {
        res = "Google";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.system.model", key.c_str()) == 0) {
        res = "mainline";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.system.name", key.c_str()) == 0) {
        res = "mainline";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.vendor.brand", key.c_str()) == 0) {
        res = "google";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.vendor.device", key.c_str()) == 0) {
        res = "coral";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.vendor.manufacturer", key.c_str()) == 0) {
        res = "Google";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.vendor.model", key.c_str()) == 0) {
        res = "Pixel 4 XL";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    } else if (strcmp("ro.product.vendor.name", key.c_str()) == 0) {
        res = "coral";
        LOGI("android::base::GetProperty: %s -> %s", key.c_str(), res.c_str());
    }
    return res;
}

void install_hook(const char *package_name, int user) {
    LOGI("install hook for %d:%s", user, package_name);

    XHOOK_REGISTER(__system_property_get);

    char sdk[PROP_VALUE_MAX + 1];
    if (__system_property_get("ro.build.version.sdk", sdk) > 0 && atoi(sdk) >= 28) {
        XHOOK_REGISTER(_ZN7android4base11GetPropertyERKNSt3__112basic_stringIcNS1_11char_traitsIcEENS1_9allocatorIcEEEES9_);
    }

    if (xhook_refresh(0) == 0)
        xhook_clear();
    else
        LOGE("failed to refresh hook");
}
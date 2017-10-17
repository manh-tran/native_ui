/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <cherry/platform.h>

#if OS == DROID

#include <native_ui/device.h>
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include "custom/util.h"

static jclass           __activity_class                                = NULL;
static jmethodID        __activity_method_get_system_service            = NULL;

static jclass           __telephony_manager_class                       = NULL;
static jmethodID        __telephony_manager_get_device_id               = NULL;

static jclass           __context_class                                  = NULL;

static void __clear()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__activity_class) {
                (*__jni_env)->DeleteGlobalRef(__jni_env, __activity_class);
                (*__jni_env)->DeleteGlobalRef(__jni_env, __telephony_manager_class);
                (*__jni_env)->DeleteGlobalRef(__jni_env, __context_class);
                __activity_class = NULL;
                __telephony_manager_class = NULL;
        }
}

static void __setup()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__activity_class == NULL) {
                cache_add(__clear);

                __activity_class = (*__jni_env)->NewGlobalRef(__jni_env,
                        (*__jni_env)->FindClass(__jni_env, "android/app/Activity"));

                __telephony_manager_class = (*__jni_env)->NewGlobalRef(__jni_env,
                        (*__jni_env)->FindClass(__jni_env, "android/telephony/TelephonyManager"));

                __context_class = (*__jni_env)->NewGlobalRef(__jni_env,
                        (*__jni_env)->FindClass(__jni_env, "android/content/Context"));

                __activity_method_get_system_service = (*__jni_env)->GetMethodID(__jni_env,
                        __activity_class,
                        "getSystemService",
                        "(Ljava/lang/String;)Ljava/lang/Object;"
                );

                __telephony_manager_get_device_id = (*__jni_env)->GetMethodID(__jni_env,
                        __telephony_manager_class,
                        "getDeviceId",
                        "()Ljava/lang/String;"
                );
        }
}

struct string *ndevice_get()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();

        jstring key = convert_to_android_utf8_string(qlkey("phone"));

        jobject telephony_manager = (*__jni_env)->CallObjectMethod(__jni_env, __activity,
                __activity_method_get_system_service, key);

        jstring jdevice_id = (jstring)(*__jni_env)->CallObjectMethod(__jni_env, telephony_manager,
                __telephony_manager_get_device_id);

        char *cstr      = (char *)(*__jni_env)->GetStringUTFChars(__jni_env, jdevice_id, NULL);
        struct string *ret = string_alloc_chars(qlkey("android_"));
        string_cat(ret, cstr, strlen(cstr));

        (*__jni_env)->ReleaseStringUTFChars(__jni_env, jdevice_id, cstr);

        (*__jni_env)->DeleteLocalRef(__jni_env, telephony_manager);
        (*__jni_env)->DeleteLocalRef(__jni_env, jdevice_id);
        (*__jni_env)->DeleteLocalRef(__jni_env, key);
        return ret;
}

#endif

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

#include "util.h"
#include <cherry/memory.h>
#include <cherry/string.h>

static jclass           __class_string                  = NULL;
static jmethodID        __method_new_string             = NULL;

static void __clear()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__class_string) {
                (*__jni_env)->DeleteGlobalRef(__jni_env, __class_string);
                __class_string = NULL;
        }
}

static void __setup()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(!__class_string) {
                cache_add(__clear);
                __class_string  = (*__jni_env)->NewGlobalRef(__jni_env, (*__jni_env)->FindClass(__jni_env, "java/lang/String"));

                 __method_new_string = (*__jni_env)->GetMethodID(__jni_env, __class_string, "<init>", "([BLjava/lang/String;)V");
        }
}

jstring convert_to_android_utf8_string(char *text, size_t len)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        jbyteArray array = (*__jni_env)->NewByteArray(__jni_env, len);
        (*__jni_env)->SetByteArrayRegion(__jni_env, array, 0, len, text);
        jstring strEncode = (*__jni_env)->NewStringUTF(__jni_env, "UTF-8");
        jstring object = (jstring) (*__jni_env)->NewObject(__jni_env, __class_string, __method_new_string, array, strEncode);

        (*__jni_env)->DeleteLocalRef(__jni_env, array);
        (*__jni_env)->DeleteLocalRef(__jni_env, strEncode);
        return object;
}

#endif

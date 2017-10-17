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
#include "textfield.h"
#include <native_ui/types.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/stdio.h>
#include "util.h"

static jclass           __class = NULL;
static jmethodID        __static_method_create  = NULL;
static jmethodID        __method_get_text = NULL;
static jmethodID        __method_set_placeholder = NULL;

static void __clear()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__class) {
                (*__jni_env)->DeleteGlobalRef(__jni_env, __class);
                __class = NULL;
        }
}

static void __setup()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__class == NULL) {
                cache_add(__clear);

                __class = (*__jni_env)->NewGlobalRef(__jni_env, (*__jni_env)->FindClass(__jni_env, "com/manhtran/nativeui/CustomTextField"));


                __static_method_create  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "create",
                        "(JLandroid/content/Context;)Lcom/manhtran/nativeui/CustomTextField;"
                );

                __method_get_text = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "getText",
                        "()Ljava/lang/String;"
                );

                __method_set_placeholder = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "setPlaceholder",
                        "(Ljava/lang/String;)V"
                );
        }
}

jobject custom_text_field_alloc(void *np)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();

        jobject obj     = (*__jni_env)->CallStaticObjectMethod(__jni_env, __class,
                                        __static_method_create, (u64)np, __activity);

        return (*__jni_env)->NewGlobalRef(__jni_env, obj);
}

struct string *custom_text_field_get_text(jobject p)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        jstring jstr    = (jstring)(*__jni_env)->CallObjectMethod(__jni_env, p, __method_get_text);

        char *cstr      = (char *)(*__jni_env)->GetStringUTFChars(__jni_env, jstr, NULL);
        struct string *ret = string_alloc_chars(cstr, strlen(cstr));

        (*__jni_env)->ReleaseStringUTFChars(__jni_env, jstr, cstr);

        return ret;
}

void custom_text_field_set_placeholder(jobject p, char *text, size_t len)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        jstring jtext = convert_to_android_utf8_string(text, len);
        (*__jni_env)->CallVoidMethod(__jni_env, p,
                __method_set_placeholder, jtext);
        (*__jni_env)->DeleteLocalRef(__jni_env, jtext);
}

#endif

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
#include "imageview.h"
#include <native_ui/types.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>

static jclass           __class = NULL;
static jmethodID        __static_method_create  = NULL;

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

                __class = (*__jni_env)->NewGlobalRef(__jni_env, (*__jni_env)->FindClass(__jni_env, "com/manhtran/nativeui/CustomImageView"));


                __static_method_create  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "create",
                        "(JLandroid/content/Context;)Lcom/manhtran/nativeui/CustomImageView;"
                );
        }
}

jobject custom_image_view_alloc(void *np)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();

        jobject obj     = (*__jni_env)->CallStaticObjectMethod(__jni_env, __class,
                                        __static_method_create, (u64)np, __activity);

        return (*__jni_env)->NewGlobalRef(__jni_env, obj);
}

#endif

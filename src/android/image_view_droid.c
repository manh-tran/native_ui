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

#include <native_ui/view.h>
#include <native_ui/view_layout_controller.h>
#include <native_ui/align.h>
#include <native_ui/parser.h>
#include <native_ui/manager.h>
#include <native_ui/image.h>
#include <native_ui/data.h>
#include <native_ui/file.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>

static jclass           __class = NULL;
static jmethodID        __static_method_set_bitmap = NULL;
static jmethodID        __static_method_load_bitmap = NULL;
static jmethodID        __static_method_clear_bitmap = NULL;

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

                __class = (*__jni_env)->NewGlobalRef(__jni_env, (*__jni_env)->FindClass(__jni_env, "com/manhtran/nativeui/CustomFunction"));

                __static_method_set_bitmap  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setBitmap",
                        "(Lcom/manhtran/nativeui/CustomImageView;Ljava/lang/String;)V"
                );

                __static_method_load_bitmap = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "loadBitmap",
                        "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;I)V"
                );

                __static_method_clear_bitmap = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "clearBitmap",
                        "(Ljava/lang/String;)V"
                );
        }
}

void native_view_on_change_imageview(struct native_view *p, char *path)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
                p->custom_data          = NULL;
                p->custom_data_free     = NULL;
        }

        p->custom_data                                  = image_view_data_alloc(path);
        p->custom_data_free                             = image_view_data_free;

        struct native_view_image_view_data *data        = (struct native_view_image_view_data *)p->custom_data;
        jstring jpath = (*__jni_env)->NewStringUTF(__jni_env, data->path->ptr);
        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_bitmap, p->ptr, jpath);
        (*__jni_env)->DeleteLocalRef(__jni_env, jpath);
}

#endif

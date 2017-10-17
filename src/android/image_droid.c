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

#include <native_ui/image.h>
#include <native_ui/file.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/string.h>

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

void nimage_init(struct nimage *p, char *name)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();

        struct string *fullpath = nfile_fullpath(name);

        jstring path = (*__jni_env)->NewStringUTF(__jni_env, name);
        jstring jfullpath = (*__jni_env)->NewStringUTF(__jni_env, fullpath->ptr);
        u8 type = nfile_type(name);
        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_load_bitmap, __activity, path, jfullpath, type);
        (*__jni_env)->DeleteLocalRef(__jni_env, path);
        (*__jni_env)->DeleteLocalRef(__jni_env, jfullpath);
        string_free(fullpath);
}

void nimage_free(struct nimage *p)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        jstring path = (*__jni_env)->NewStringUTF(__jni_env, p->path->ptr);
        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_clear_bitmap, path);
        (*__jni_env)->DeleteLocalRef(__jni_env, path);

        nimage_free_common(p);
        sfree(p);
}

#endif

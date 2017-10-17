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
#include <native_ui/font.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>
#include "custom/label.h"
#include "custom/util.h"

static jclass           __class                         = NULL;
static jclass           __class_string                  = NULL;
static jmethodID        __static_method_create          = NULL;
static jmethodID        __method_set_font_size          = NULL;
static jmethodID        __method_set_multi_line         = NULL;
static jmethodID        __method_set_text_alignment     = NULL;
static jmethodID        __method_set_text               = NULL;
static jmethodID        __method_calculate_height       = NULL;
static jmethodID        __method_set_text_color         = NULL;

static void __clear()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__class) {
                (*__jni_env)->DeleteGlobalRef(__jni_env, __class);
                (*__jni_env)->DeleteGlobalRef(__jni_env, __class_string);
                __class = NULL;
        }
}

static void __setup()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__class == NULL) {
                cache_add(__clear);

                __class         = (*__jni_env)->NewGlobalRef(__jni_env, (*__jni_env)->FindClass(__jni_env, "com/manhtran/nativeui/CustomLabel"));
                __class_string  = (*__jni_env)->NewGlobalRef(__jni_env, (*__jni_env)->FindClass(__jni_env, "java/lang/String"));

                __static_method_create  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "create",
                        "(JLandroid/content/Context;)Lcom/manhtran/nativeui/CustomLabel;"
                );

                __method_set_font_size  = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "setFontSize",
                        "(F)V"
                );

                __method_set_multi_line = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "setMultiline",
                        "(I)V"
                );

                __method_set_text_alignment = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "setTextAlignment",
                        "(I)V"
                );

                __method_set_text  = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "setText",
                        "(Ljava/lang/String;)V"
                );

                __method_calculate_height  = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "calculateHeight",
                        "()I"
                );

                __method_set_text_color = (*__jni_env)->GetMethodID(__jni_env,
                        __class,
                        "setTextColor",
                        "(FFFF)V"
                );
        }
}


void nview_on_change_label(struct nview *p)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
                p->custom_data          = NULL;
                p->custom_data_free     = NULL;
        }

        p->custom_data                          = nview_label_data_alloc();
        p->custom_data_free                     = nview_label_data_free;

        struct nview_label_data *data     = (struct nview_label_data *)p->custom_data;
        data->font                              = native_ui_font_get(qlkey("arial"), 14);

        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_font_size, data->font->size);
        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_text_color, 0.0f, 0.0f, 0.0f, 1.0f);
        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_multi_line, data->multi_line);

        nview_update_label(p);
        nview_set_text_align(p, NATIVE_UI_LABEL_ALIGN_LEFT);
}

void nview_update_label(struct nview *p)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->type != NATIVE_UI_LABEL) return;

        struct nview_label_data *data     = (struct nview_label_data *)p->custom_data;

        /*
         * android layout does not update immediately so I can not get real size of inner label
         * to resize {p}
         * size of {p} will be updated from viewSizeJNI call in src/android/view.c
         */
         if(data->dirty || fabsf(p->size.width - data->current_width) > 1) {
                 int h = (*__jni_env)->CallIntMethod(__jni_env, p->ptr,
                         __method_calculate_height);

                 union vec2 size;
                 size.width      = p->size.width;
                 size.height     = h;

                 nview_set_size(p, size);

                 p->align->fixed_width                   = size.width;
                 p->align->fixed_height                  = h;

                 data->dirty                             = 0;
                 data->current_width                     = p->size.width;
         }
}

void nview_set_text_align(struct nview *p, u8 text_align)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->type != NATIVE_UI_LABEL) return;

        struct nview_label_data *data     = (struct nview_label_data *)p->custom_data;
        data->align                             = text_align;

        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_text_alignment, (int)text_align);

        nview_request_layout(p);
}

void nview_set_text_multiline(struct nview *p, u8 multiline)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->type != NATIVE_UI_LABEL) return;

        struct nview_label_data *data     = (struct nview_label_data *)p->custom_data;
        data->multi_line = multiline;

        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_multi_line, data->multi_line);

        nview_request_layout(p);
}

void nview_set_text(struct nview *p, char *text, size_t len)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->type != NATIVE_UI_LABEL) return;
        struct nview_label_data *data     = (struct nview_label_data *)p->custom_data;

        data->text->len                         = 0;
        string_cat(data->text, text, len);

        jstring jtext = convert_to_android_utf8_string(text, len);
        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_text, jtext);
        (*__jni_env)->DeleteLocalRef(__jni_env, jtext);

        nview_request_layout(p);
}

void nview_set_text_color(struct nview *p, union vec4 color)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->type != NATIVE_UI_LABEL) return;

        struct nview_label_data *data     = (struct nview_label_data *)p->custom_data;
        data->color                             = color;

        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_text_color, color.r, color.g, color.b, color.a);
}

void nview_set_font(struct nview *p, char *font_name, size_t len, size_t size)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        __setup();
        if(p->type != NATIVE_UI_LABEL) return;

        struct nview_label_data *data     = (struct nview_label_data *)p->custom_data;
        data->font->size                        = size;

        (*__jni_env)->CallVoidMethod(__jni_env, p->ptr,
                __method_set_font_size, data->font->size);

        nview_request_layout(p);
}

#endif

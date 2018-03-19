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
#include <native_ui/touch.h>
#include <native_ui/manager.h>

#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/stdio.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/math/math.h>

#include "custom/view.h"
#include "custom/imageview.h"
#include "custom/textfield.h"
#include "custom/textview.h"
#include "custom/label.h"

static jclass           __class = NULL;
static jmethodID        __static_method_set_position = NULL;
static jmethodID        __static_method_set_size = NULL;
static jmethodID        __static_method_set_rotation = NULL;
static jmethodID        __static_method_set_scale = NULL;
static jmethodID        __static_method_set_color = NULL;
static jmethodID        __static_method_set_color_null = NULL;
static jmethodID        __static_method_set_alpha = NULL;
static jmethodID        __static_method_set_visible = NULL;
static jmethodID        __static_method_set_clip = NULL;
static jmethodID        __static_method_set_anchor = NULL;
static jmethodID        __static_method_set_touch_enabled = NULL;
static jmethodID        __static_method_convert_point = NULL;
static jmethodID        __static_method_add_child = NULL;
static jmethodID        __static_method_remove_from_parent = NULL;

pthread_mutex_t         __shared_native_view_touch_lock;

static void __clear()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__class) {
                (*__jni_env)->DeleteGlobalRef(__jni_env, __class);
                __class = NULL;

                pthread_mutex_destroy(&__shared_native_view_touch_lock);
        }
}

static void __setup()
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(__class == NULL) {
                cache_add(__clear);

                pthread_mutex_init(&__shared_native_view_touch_lock, NULL);

                __class = (*__jni_env)->NewGlobalRef(__jni_env, (*__jni_env)->FindClass(__jni_env, "com/manhtran/nativeui/CustomFunction"));

                __static_method_remove_from_parent  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "removeFromParent",
                        "(Landroid/widget/AbsoluteLayout;)V"
                );

                __static_method_add_child  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "addChild",
                        "(Landroid/widget/AbsoluteLayout;Landroid/widget/AbsoluteLayout;)V"
                );

                __static_method_set_position  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setPosition",
                        "(Lcom/manhtran/nativeui/CustomSharedView;FF)V"
                );

                __static_method_set_size  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setSize",
                        "(Lcom/manhtran/nativeui/CustomSharedView;FF)V"
                );

                __static_method_set_rotation  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setRotation",
                        "(Lcom/manhtran/nativeui/CustomSharedView;FFF)V"
                );

                __static_method_set_scale  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setScale",
                        "(Lcom/manhtran/nativeui/CustomSharedView;FF)V"
                );

                __static_method_set_color  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setColor",
                        "(Lcom/manhtran/nativeui/CustomSharedView;FFFFFFFF)V"
                );

                __static_method_set_color_null  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setColorNull",
                        "(Lcom/manhtran/nativeui/CustomSharedView;)V"
                );

                __static_method_set_alpha  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setAlpha",
                        "(Lcom/manhtran/nativeui/CustomSharedView;F)V"
                );

                __static_method_set_visible  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setVisible",
                        "(Lcom/manhtran/nativeui/CustomSharedView;I)V"
                );

                __static_method_set_anchor  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setAnchor",
                        "(Lcom/manhtran/nativeui/CustomSharedView;FF)V"
                );

                __static_method_set_clip  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setClip",
                        "(Lcom/manhtran/nativeui/CustomSharedView;IFFFF)V"
                );

                __static_method_convert_point  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "convertPoint",
                        "(Landroid/view/View;IILandroid/view/View;)[I"
                );

                __static_method_set_touch_enabled  = (*__jni_env)->GetStaticMethodID(__jni_env,
                        __class,
                        "setTouchEnabled",
                        "(Lcom/manhtran/nativeui/CustomSharedView;I)V"
                );
        }
}

struct native_view *native_view_alloc()
{
        __setup();

        struct native_view *p           = smalloc(sizeof(struct native_view), native_view_free);
        native_view_init(p);

        p->ptr                          = NULL;

        native_view_show_view(p);
        return p;
}

void native_view_free(struct native_view *p)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        native_view_free_common(p);
        /*
         * deallocate view content
         */
        if(p->ptr) {
                (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                        __static_method_remove_from_parent, p->ptr);
                (*__jni_env)->DeleteGlobalRef(__jni_env, p->ptr);
        }
        sfree(p);
}

static void __change_view(struct native_view *p, jobject new_view)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(p->ptr) {
                (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                        __static_method_remove_from_parent, p->ptr);
                (*__jni_env)->DeleteGlobalRef(__jni_env, p->ptr);
                p->ptr  = NULL;
        }

        p->ptr = new_view;

        if(p->parent && p->parent->ptr) {
                (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                        __static_method_add_child, p->parent->ptr, p->ptr);
        }
}

/*
 * show views
 */
void native_view_show_view(struct native_view *p)
{
        p->type                 = NATIVE_UI_VIEW;
        jobject view            = custom_view_alloc(p);
        __change_view(p, view);

        native_view_set_position(p, p->position);
        native_view_set_size(p, p->size);
        native_view_set_scale(p, p->scale);
        native_view_set_rotation(p, p->rotation);
        native_view_set_clip(p, p->clip);
        native_view_set_color(p, p->color, p->border_color);
        native_view_set_alpha(p, p->alpha);
        native_view_set_anchor(p, p->anchor);
        native_view_set_visible(p, p->visible);
        native_view_set_user_interaction_enabled(p, p->user_interaction_enabled);

}

void native_view_show_image(struct native_view *p, char *path)
{
        p->type                 = NATIVE_UI_IMAGE;
        jobject view            = custom_image_view_alloc(p);
        __change_view(p, view);

        native_view_set_position(p, p->position);
        native_view_set_size(p, p->size);
        native_view_set_scale(p, p->scale);
        native_view_set_rotation(p, p->rotation);
        native_view_set_clip(p, p->clip);
        native_view_set_color(p, p->color, p->border_color);
        native_view_set_alpha(p, p->alpha);
        native_view_set_anchor(p, p->anchor);
        native_view_set_visible(p, p->visible);
        native_view_set_user_interaction_enabled(p, p->user_interaction_enabled);

        native_view_on_change_imageview(p, path);
}

void native_view_show_label(struct native_view *p)
{
        p->type                 = NATIVE_UI_LABEL;
        jobject view            = custom_label_alloc(p);
        __change_view(p, view);

        native_view_set_position(p, p->position);
        native_view_set_size(p, p->size);
        native_view_set_scale(p, p->scale);
        native_view_set_rotation(p, p->rotation);
        native_view_set_clip(p, p->clip);
        native_view_set_color(p, p->color, p->border_color);
        native_view_set_alpha(p, p->alpha);
        native_view_set_anchor(p, p->anchor);
        native_view_set_visible(p, p->visible);
        native_view_set_user_interaction_enabled(p, p->user_interaction_enabled);

        native_view_on_change_label(p);
}

void native_view_show_textfield(struct native_view *p)
{
        p->type                 = NATIVE_UI_TEXTFIELD;
        jobject view            = custom_text_field_alloc(p);
        __change_view(p, view);

        native_view_set_position(p, p->position);
        native_view_set_size(p, p->size);
        native_view_set_scale(p, p->scale);
        native_view_set_rotation(p, p->rotation);
        native_view_set_clip(p, p->clip);
        native_view_set_color(p, p->color, p->border_color);
        native_view_set_alpha(p, p->alpha);
        native_view_set_anchor(p, p->anchor);
        native_view_set_visible(p, p->visible);
        native_view_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void native_view_show_textview(struct native_view *p)
{
        p->type                 = NATIVE_UI_TEXTVIEW;
        jobject view            = custom_text_view_alloc(p);
        __change_view(p, view);

        native_view_set_position(p, p->position);
        native_view_set_size(p, p->size);
        native_view_set_scale(p, p->scale);
        native_view_set_rotation(p, p->rotation);
        native_view_set_clip(p, p->clip);
        native_view_set_color(p, p->color, p->border_color);
        native_view_set_alpha(p, p->alpha);
        native_view_set_anchor(p, p->anchor);
        native_view_set_visible(p, p->visible);
        native_view_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void native_view_show_listview(struct native_view *p)
{
        p->type                 = NATIVE_UI_LISTVIEW;
        jobject view            = custom_view_alloc(p);
        __change_view(p, view);

        native_view_set_position(p, p->position);
        native_view_set_size(p, p->size);
        native_view_set_scale(p, p->scale);
        native_view_set_rotation(p, p->rotation);
        native_view_set_clip(p, p->clip);
        native_view_set_color(p, p->color, p->border_color);
        native_view_set_alpha(p, p->alpha);
        native_view_set_anchor(p, p->anchor);
        native_view_set_visible(p, p->visible);
        native_view_set_user_interaction_enabled(p, p->user_interaction_enabled);

        native_view_on_change_listview(p);
}

/*
 * parent child
 */
void native_view_add_child(struct native_view *p, struct native_view *c)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(c->parent && c->parent->name_to_child) {
                map_remove_key(c->parent->name_to_child, qskey(c->name));
        }

        list_del(&c->head);
        list_add_tail(&c->head, &p->children);
        c->parent = p;

        if(c->name->len) {
                if(!p->name_to_child) {
                        p->name_to_child = map_alloc(sizeof(struct native_view *));
                        map_set(p->name_to_child, qskey(c->name), &c);
                }
        }

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_remove_from_parent, c->ptr);

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_add_child, p->ptr, c->ptr);

        native_view_set_position(c, c->position);
        native_view_set_size(c, c->size);
        native_view_set_scale(c, c->scale);
        native_view_set_rotation(c, c->rotation);
        native_view_set_clip(c, c->clip);
        native_view_set_color(c, c->color, c->border_color);
        native_view_set_alpha(c, c->alpha);
        native_view_set_anchor(c, c->anchor);
        native_view_set_visible(c, c->visible);
        native_view_set_user_interaction_enabled(c, c->user_interaction_enabled);
}

void native_view_remove_from_parent(struct native_view *p)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(!p->parent) return;

        if(p->parent && p->parent->name_to_child) {
                map_remove_key(p->parent->name_to_child, qskey(p->name));
        }

        list_del_init(&p->head);
        p->parent = NULL;

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_remove_from_parent, p->ptr);
}

/*
 * set transform
 */
void native_view_set_position(struct native_view *p, union vec2 position)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->position     = position;

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_position, p->ptr, position.x, position.y);
}

void native_view_set_size(struct native_view *p, union vec2 size)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->size         = size;
        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_size, p->ptr, size.x, size.y);
}

void native_view_set_scale(struct native_view *p, union vec2 scale)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->scale                        = scale;
        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_scale, p->ptr, scale.x, scale.y);
}

void native_view_set_rotation(struct native_view *p, union vec3 rotation)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->rotation                     = rotation;

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_rotation, p->ptr, rotation.x, rotation.y, rotation.z);
}

void native_view_set_color(struct native_view *p, union vec4 *color, union vec4 *border)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        if(color) {
                if(!p->color) {
                        p->color        = smalloc(sizeof(union vec4), sfree);
                }
                (*p->color)             = (*color);
        } else {
                if(p->color) {
                        sfree(p->color);
                        p->color        = NULL;
                }
        }

        if(border) {
                if(!p->border_color) {
                        p->border_color = smalloc(sizeof(union vec4), sfree);
                }
                (*p->border_color)      = (*border);
        } else {
                if(p->border_color) {
                        sfree(p->border_color);
                        p->border_color = NULL;
                }
        }

        if(p->color && p->border_color) {
                (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                        __static_method_set_color, p->ptr, color->r, color->g, color->b, color->a,
                                border->r, border->g, border->b, border->a);
        } else if(p->color) {
                (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                        __static_method_set_color, p->ptr, color->r, color->g, color->b, color->a,
                                -1.0f, -1.0f, -1.0f, -1.0f);
        } else if(p->border_color) {
                (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                        __static_method_set_color, p->ptr, -1.0f, -1.0f, -1.0f, -1.0f,
                                border->r, border->g, border->b, border->a);
        } else {
                (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                        __static_method_set_color, p->ptr, -1.0f, -1.0f, -1.0f, -1.0f,
                                -1.0f, -1.0f, -1.0f, -1.0f);
        }
}

void native_view_set_visible(struct native_view *p, u8 visible)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->visible = visible;

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_visible, p->ptr, (int)visible);
}

void native_view_set_alpha(struct native_view *p, float alpha)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->alpha                        = alpha;

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_alpha, p->ptr, alpha);
}

void native_view_set_clip(struct native_view *p, u8 clip)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->clip                         = clip;
        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_clip, p->ptr, (int)clip, p->clip_rounds.x, p->clip_rounds.y, p->clip_rounds.z, p->clip_rounds.w);
}

void native_view_set_anchor(struct native_view *p, union vec2 anchor)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->anchor                       = anchor;

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_anchor, p->ptr, anchor.x, anchor.y);
}

/*
 * touches
 */
void native_view_set_user_interaction_enabled(struct native_view *p, u8 enabled)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        p->user_interaction_enabled     = enabled;

        (*__jni_env)->CallStaticVoidMethod(__jni_env, __class,
                __static_method_set_touch_enabled, p->ptr, (int)enabled);
}

union vec2 native_view_convert_point_to_view(struct native_view *from, union vec2 point, struct native_view *to)
{
        JNIEnv *__jni_env = __jni_env_current_thread();
        union vec2 ret;

        jintArray arr   = (*__jni_env)->CallStaticObjectMethod(__jni_env, __class,
                __static_method_convert_point, from->ptr, (int)point.x, (int)point.y, to->ptr);
        jsize len       = (*__jni_env)->GetArrayLength(__jni_env, arr);
        jint *body      = (*__jni_env)->GetIntArrayElements(__jni_env, arr, 0);

        ret.x           = body[0] * 1.0f;
        ret.y           = body[1] * 1.0f;

        (*__jni_env)->ReleaseIntArrayElements(__jni_env, arr, body, 0);

        return ret;
}

/*
 * currently multi touches are disabled
 */
static u8 __view_touched = 0;
static u8 __view_touched_move = 0;
static u8 __view_touched_end = 0;

/*
 * JNI
 */
JNIEXPORT int JNICALL Java_com_manhtran_nativeui_CustomFunction_touchBeganJNI(
        JNIEnv *env,
        jobject th,
        u64 ptr, int index, float x, float y, float sx, float sy)
{
        __setup();
        if(__view_touched) return 0;

        pthread_mutex_lock(&__shared_native_view_touch_lock);

        struct native_view *view = (struct native_view *)ptr;

        struct native_view *parent = view->parent;
        while(parent) {
                if(parent->clip) {
                        union vec2 tl = native_view_get_screen_pos(parent, (union vec2){0, 0});
                        union vec2 pt = vec2_sub((union vec2){sx, sy}, tl);
                        if(pt.x < 0 || pt.x > parent->size.width || pt.y < 0 || pt.y > parent->size.height) {
                                /*
                                 * prevent receiving outside touch
                                 */
                                pthread_mutex_unlock(&__shared_native_view_touch_lock);
                                return 0;
                        }
                }
                parent = parent->parent;
        }

        if(x >= 0 && x <= view->size.width && y >= 0 && y <= view->size.height) {
                struct native_view_touch_data *data = native_view_touch_data_alloc();
                list_add_tail(&data->head, &view->touch_list);
                list_add_tail(&data->shared_head, &native_manager_shared()->touches);
                data->view = view;
                data->type = NATIVE_UI_TOUCH_BEGAN;
                data->point = (union vec2){x, y};
                data->pscr = (union vec2){sx, sy};
                // native_view_touch_began((struct native_view *)ptr, (union vec2){x, y});
                __view_touched = 1;
                pthread_mutex_unlock(&__shared_native_view_touch_lock);
                return 1;
        } else {
                /*
                 * prevent receiving outside touch
                 */
                pthread_mutex_unlock(&__shared_native_view_touch_lock);
                return 0;
        }
}

JNIEXPORT void JNICALL Java_com_manhtran_nativeui_CustomFunction_touchMovedJNI(
        JNIEnv *env,
        jobject th,
        u64 ptr, int index, float x, float y, float sx, float sy)
{
        __setup();

        pthread_mutex_lock(&__shared_native_view_touch_lock);
        struct native_view *view = (struct native_view *)ptr;
        struct native_view_touch_data *data = native_view_touch_data_alloc();
        list_add_tail(&data->head, &view->touch_list);
        list_add_tail(&data->shared_head, &native_manager_shared()->touches);
        data->type = NATIVE_UI_TOUCH_MOVED;
        data->point = (union vec2){x, y};
        data->pscr = (union vec2){sx, sy};
        data->view = view;
        pthread_mutex_unlock(&__shared_native_view_touch_lock);

        // native_view_touch_moved((struct native_view *)ptr, (union vec2){x, y});
}

JNIEXPORT void JNICALL Java_com_manhtran_nativeui_CustomFunction_touchEndedJNI(
        JNIEnv *env,
        jobject th,
        u64 ptr, int index, float x, float y, float sx, float sy)
{
        __setup();
        if(__view_touched_end == 0) {
                __view_touched_end = 1;
                pthread_mutex_lock(&__shared_native_view_touch_lock);
                struct native_view *view = (struct native_view *)ptr;
                struct native_view_touch_data *data = native_view_touch_data_alloc();
                list_add_tail(&data->head, &view->touch_list);
                list_add_tail(&data->shared_head, &native_manager_shared()->touches);
                data->type = NATIVE_UI_TOUCH_ENDED;
                data->point = (union vec2){x, y};
                data->pscr = (union vec2){sx, sy};
                data->view = view;
                pthread_mutex_unlock(&__shared_native_view_touch_lock);

                // native_view_touch_ended((struct native_view *)ptr, (union vec2){x, y});
        }
        __view_touched_end = 0;
        __view_touched = 0;
}

JNIEXPORT void JNICALL Java_com_manhtran_nativeui_CustomFunction_touchCancelledJNI(
        JNIEnv *env,
        jobject th,
        u64 ptr, int index, float x, float y, float sx, float sy)
{
        __setup();
        if(__view_touched_end == 0) {
                __view_touched_end = 1;

                pthread_mutex_lock(&__shared_native_view_touch_lock);
                struct native_view *view = (struct native_view *)ptr;
                struct native_view_touch_data *data = native_view_touch_data_alloc();
                list_add_tail(&data->head, &view->touch_list);
                list_add_tail(&data->shared_head, &native_manager_shared()->touches);
                data->type = NATIVE_UI_TOUCH_CANCELLED;
                data->point = (union vec2){x, y};
                data->pscr = (union vec2){sx, sy};
                data->view = view;
                pthread_mutex_unlock(&__shared_native_view_touch_lock);

                // native_view_touch_cancelled((struct native_view *)ptr, (union vec2){x, y});
        }
        __view_touched_end = 0;
        __view_touched = 0;
}

JNIEXPORT void JNICALL Java_com_manhtran_nativeui_CustomFunction_textDoneJNI(
        JNIEnv *env,
        jobject th,
        u64 ptr)
{
        __setup();
        native_view_text_done((struct native_view *)ptr);
}

JNIEXPORT void JNICALL Java_com_manhtran_nativeui_CustomFunction_viewResizeJNI(
        JNIEnv *env,
        jobject th,
        u64 ptr, float width, float height)
{
        __setup();
        struct native_view *view = (struct native_view *)ptr;
        if(view->type == NATIVE_UI_LABEL) {
                union vec2 size;
                size.width      = view->size.width;
                size.height     = height;
                /*
                 * set view's size directly. Don't call native_view_set_size function to prevent infinite loop
                 */
                view->size      = size;
                view->align->fixed_width        = size.width;
                view->align->fixed_height       = height;
                // native_view_request_size(view, size);
        } else {

        }
}

#endif

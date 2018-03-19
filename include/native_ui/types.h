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
#ifndef __NATIVE_UI_TYPES_H__
#define __NATIVE_UI_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cherry/types.h>
#include <cherry/math/types.h>
#include <pthread.h>

struct native_view;
struct native_controller;
struct native_touch;

#define INNER_PATH "res/"
#define LOCAL_PATH "local/"
#define SD_PATH "sd/"

struct npref
{
        struct sobj     *data;
        struct string           *key;
};

#if OS == IOS
        extern struct native_view *__root;
#endif

#if OS == DROID
        #include <jni.h>
        extern jobject __activity;

        extern JavaVM* __jvm;
        extern struct map *__jni_env_map;

        JNIEnv *__jni_env_current_thread();
#endif

typedef void(*custom_data_free_delegate)(void*);
typedef void(*user_data_free_delegate)(void*);
/*
 * native ui manager
 */
typedef void(*native_task_delegate)(void *, float);

struct native_task {
        struct list_head                head;
        struct list_head                user_head;
        void                            *data;
        native_task_delegate  delegate;
        int                             count;
};

/*
 * image cache
 */
struct nimage {

#if OS == IOS
        void                    *ptr;
#endif

        struct string           *path;
        struct list_head        links;
};

/*
 * actions
 */

enum {
        NATIVE_UI_EASE_LINEAR,
        NATIVE_UI_EASE_QUADRATIC_IN,
        NATIVE_UI_EASE_QUADRATIC_OUT,
        NATIVE_UI_EASE_QUADRATIC_IN_OUT,
        NATIVE_UI_EASE_CUBIC_IN,
        NATIVE_UI_EASE_CUBIC_OUT,
        NATIVE_UI_EASE_CUBIC_IN_OUT,
        NATIVE_UI_EASE_QUARTIC_IN,
        NATIVE_UI_EASE_QUARTIC_OUT,
        NATIVE_UI_EASE_QUARTIC_IN_OUT,
        NATIVE_UI_EASE_QUINTIC_IN,
        NATIVE_UI_EASE_QUINTIC_OUT,
        NATIVE_UI_EASE_QUINTIC_IN_OUT,
        NATIVE_UI_EASE_SINUSOIDAL_IN,
        NATIVE_UI_EASE_SINUSOIDAL_OUT,
        NATIVE_UI_EASE_SINUSOIDAL_IN_OUT,
        NATIVE_UI_EASE_EXPONENTIAL_IN,
        NATIVE_UI_EASE_EXPONENTIAL_OUT,
        NATIVE_UI_EASE_EXPONENTIAL_IN_OUT,
        NATIVE_UI_EASE_CIRCULAR_IN,
        NATIVE_UI_EASE_CIRCULAR_OUT,
        NATIVE_UI_EASE_CIRCULAR_IN_OUT,

        NATIVE_UI_EASE_GRAVITY,
        NATIVE_UI_EASE_SEQUENCE,
        NATIVE_UI_EASE_PARALLEL,

        NATIVE_UI_EASE_FORCE,
        NATIVE_UI_EASE_DELAY
};

typedef void(*nactionf)(void*);

struct naction {
        struct list_head        user_head;
        struct list_head        head;
        struct list_head        children;

        u8                      ease_type;

        union vec4              *target;
        union {
                /*
                 * ease action
                 */
                struct {
                        union vec4      offset;
                        union vec4      last_amount_offset;

                        float           duration;
                        float           remain;
                };
                /*
                 * gravity action
                 */
                struct {
                        float           velocity;
                        float           max_velocity;
                        float           accelerate;

                        struct array    *destinations;

                        i16             index;
                };
                /*
                 * immediately action
                 */
                struct {
                        union vec4      destination;
                };
                /*
                 * delay action
                 */
                struct {
                        float           delay;
                };
        };

        nactionf       callback;
        void                            *callback_data;

        u8                              finish;

        i16                             repeat;
};

struct naction_key {
        struct list_head                key_head;
        struct list_head                actions;
};

/*
 * manager
 */
struct native_manager {
        struct list_head                update_tasks;
        struct list_head                action_keys;
        struct list_head                touches;
};

/*
 * native controller
 */
typedef void(*native_controller_olf)(struct native_controller *);
typedef void(*native_controller_orf)(struct native_controller *);
typedef void(*native_controller_ouf)(struct native_controller *, float);
struct native_controller
{
        struct list_head                                view;

        struct list_head                                children;
        struct list_head                                head;
        struct native_controller                   *parent;

        native_controller_olf       on_linked;
        native_controller_orf      on_removed;
        native_controller_ouf       on_update;

        /*
         * schedule tasks
         */
        struct map                                      *tasks;

        void                                            *custom_data;
        custom_data_free_delegate                       custom_data_free;

        struct pool_list_head   anonymous;
};

/*
 * native view
 */
enum {
        NATIVE_UI_VIEW,
        NATIVE_UI_IMAGE,
        NATIVE_UI_LABEL,
        NATIVE_UI_TEXTFIELD,
        NATIVE_UI_TEXTVIEW,
        NATIVE_UI_LISTVIEW,
        NATIVE_UI_PAGEVIEW
};

typedef void(*native_controller_tbeganf)(struct native_view *, void* data, int index, union vec2);
typedef void(*native_controller_tmovedf)(struct native_view *, void* data, int index, union vec2);
typedef void(*native_controller_tendedf)(struct native_view *, void* data, int index, union vec2);
typedef void(*native_controller_tcancelledf)(struct native_view *, void* data, int index, union vec2);
typedef void(*native_controller_txtf)(struct native_view *);

enum {
        NATIVE_UI_LAYOUT_RELATIVE,
        NATIVE_UI_LAYOUT_VERTICAL,
        NATIVE_UI_LAYOUT_HORIZONTAL,
        NATIVE_UI_LAYOUT_ABSOLUTE,
        NATIVE_UI_LAYOUT_SIZE_VERTICAL,
        NATIVE_UI_LAYOUT_SIZE_HORIZONTAL
};

struct native_layout {
        u8                      type;
        struct list_head        head;
};

enum {
        NATIVE_UI_ALIGN_TOP             = 1 << 0,
        NATIVE_UI_ALIGN_RIGHT           = 1 << 1,
        NATIVE_UI_ALIGN_BOTTOM          = 1 << 2,
        NATIVE_UI_ALIGN_LEFT            = 1 << 3,

        NATIVE_UI_ALIGN_OUT_TOP         = 1 << 4,
        NATIVE_UI_ALIGN_OUT_RIGHT       = 1 << 5,
        NATIVE_UI_ALIGN_OUT_BOTTOM      = 1 << 6,
        NATIVE_UI_ALIGN_OUT_LEFT        = 1 << 7,

        NATIVE_UI_ALIGN_CENTER          = 1 << 8,

        NATIVE_UI_ALIGN_SIZE_FIXED      = 1 << 9,
        NATIVE_UI_ALIGN_SIZE_DYNAMIC    = 1 << 10
};

enum {
        NATIVE_UI_UPDATE_SELF           = 1 << 0,
        NATIVE_UI_UPDATE_CHILDREN       = 1 << 1
};

enum {
        NATIVE_UI_SIZE_MATCH_PARENT     = 1,
        NATIVE_UI_SIZE_WRAP_CONTENT     = 2,
        NATIVE_UI_SIZE_FIXED            = 3,
        NATIVE_UI_SIZE_FILL             = 4,
        NATIVE_UI_SIZE_EQUAL            = 5,
        NATIVE_UI_SIZE_WEIGHT           = 6
};

struct native_view_align {
        u64                                     type;
        u8                                      size_width;
        u8                                      size_height;
        union {
                float                           fixed_width;
                float                           percent_width;
        };
        union {
                float                           fixed_height;
                float                           percent_height;
        };
        int                                     weight;
        union vec4                              margin;
        union vec4                              padding;
};

struct native_view {
        int                                     type;
#if OS == IOS
        void                                    *ptr;
#endif

#if OS == DROID
        jobject                                 ptr;
#endif

#if OS == WEB
        i32                                     ptr;
#endif
        /*
         * parent-child relationships
         */
        struct list_head                        head;
        struct list_head                        children;
        struct native_view                      *parent;

        struct map                              *name_to_child;

        /*
         * layout controller
         */
        struct list_head                        layout_controller;
        struct native_view_align                *align;
        u32                                     update;

        /*
         * user controller
         */
        struct list_head                        view_controller;

        /*
         * basic content
         */
        union {
                union vec2                      size;
                union vec4                      size_expanded;
        };
        union {
                union vec2                      position;
                union vec4                      position_expanded;
        };
        union {
                union vec2                      scale;
                union vec4                      scale_expanded;
        };
        union vec4                              *color;
        union vec4                              *border_color;
        union {
                float                           alpha;
                union vec4                      alpha_expanded;
        };
        union {
                union vec3                      rotation;
                union vec4                      rotation_expanded;
        };
        union vec2                              anchor;
        union vec2                              offset_child;
        union vec2                              bound;
        u8                                      clip;
        union vec4                              clip_rounds;
        struct string                           *name;
        u8                                      visible;
        float                                   weights;

        /*
         * touch
         */
        u8                                      user_interaction_enabled;
        native_controller_tbeganf               touch_began_delegate;
        native_controller_tmovedf               touch_moved_delegate;
        native_controller_tendedf               touch_ended_delegate;
        native_controller_tcancelledf           touch_cancelled_delegate;
        native_controller_txtf                  text_done_delegate;

        union vec2                              touch_began_point;
        union vec2                              touch_moved_point;
        union vec2                              touch_ended_point;
        union vec2                              touch_offset;

        /*
         * parser
         */
        struct list_head                        parser;
        struct list_head                        hash_head;

        /*
         * user define data
         */
        void                                    *custom_data;
        custom_data_free_delegate               custom_data_free;

        void                                    *user_data;
        user_data_free_delegate                 user_data_free;

        void                                    *touch_data;
        void(*touch_data_free)(void *);

        /*
         * schedule tasks
         */
        struct map                              *tasks;

        /*
         * parser touch delegates
         */
        struct native_touch                     *parser_touch_handle;

        struct list_head                        intercept_horizontal;
        struct list_head                        intercept_horizontal_head;
        struct native_view                      *intercept_horizontal_target;

        struct list_head                        intercept_vertical;
        struct list_head                        intercept_vertical_head;
        struct native_view                      *intercept_vertical_target;

        /*
         * action key
         */
        struct naction_key                      action_key;

        struct list_head                        touch_list;
};

extern pthread_mutex_t  __shared_native_view_touch_lock;

struct native_view_touch_data
{
        struct list_head        shared_head;
        struct list_head        head;
        struct native_view            *view;
        u8                      type;
        union vec2              point;
        union vec2              pscr;
};

struct native_view_list_view_data {
        u8                                      hold;
        u8                                      bouncing;
        u8                                      stop;
        u8                                      scrolling;
        union vec2                              velocity;
        union vec2                              acceleration;
        union vec2                              last_moved_point;
};

struct native_view_page_view_data {
        u8                                      hold;
        u8                                      bouncing;
        u8                                      stop;
        u8                                      scrolling;
        union vec2                              velocity;
        union vec2                              acceleration;
        union vec2                              last_moved_point;
};

struct native_view_image_view_data {
        struct list_head        image_link;
        struct string           *path;
};

struct native_ui_font {
        struct string   *name;
        float           size;
#if OS == IOS
        void            *ptr;
#endif
};

enum {
        NATIVE_UI_LABEL_ALIGN_LEFT,
        NATIVE_UI_LABEL_ALIGN_RIGHT,
        NATIVE_UI_LABEL_ALIGN_CENTER,
        NATIVE_UI_LABEL_ALIGN_JUSTIFIED
};

struct native_view_label_data {
        struct string           *text;
        u8                      multi_line;
        struct native_ui_font   *font;
        union vec4              color;
        u8                      editable;
        u8                      align;
        u8                      dirty;
        float                   current_width;
};

enum {
        NATIVE_UI_TOUCH_BEGAN,
        NATIVE_UI_TOUCH_MOVED,
        NATIVE_UI_TOUCH_ENDED,
        NATIVE_UI_TOUCH_CANCELLED
};

typedef void(*native_touch_delegate)(void*, void*, u8);
struct native_touch {

        struct list_head                        children;
        struct list_head                        head;
        struct native_touch         *parent;

        native_touch_delegate       touch_delegate;
        void                                    *custom_data;
        custom_data_free_delegate               custom_data_free;

        u8                                      execute;
};

struct native_parser {
        struct list_head                        view;

        struct map                              *hash_views;
        struct map                              *hash_touches;
        struct map                              *hash_templates;

        struct native_controller           *controller;

        struct native_parser               *parent;

        int                                     template_scope;
};

#define controller_get_viewf1(a) native_controller_get_view(a)
#define controller_get_viewf2(a, b) struct native_view *a = native_controller_get_view(b)
#define controller_get_viewGET_MACRO(_1,_2,NAME,...) NAME
#define controller_get_view(...) controller_get_viewGET_MACRO(__VA_ARGS__,controller_get_viewf2,controller_get_viewf1)(__VA_ARGS__)

#define view_get_parserf1(a) native_view_get_parser(a)
#define view_get_parserf2(a, b) struct native_parser *a = native_view_get_parser(b)
#define view_get_parserGET_MACRO(_1,_2,NAME,...) NAME
#define view_get_parser(...) view_get_parserGET_MACRO(__VA_ARGS__,view_get_parserf2,view_get_parserf1)(__VA_ARGS__)

#define parser_hash_viewf2(a, b, c) native_parser_get_hash_view(a, b, c)
#define parser_hash_viewf3(a, b, c, d) struct native_view *a = native_parser_get_hash_view(b, c, d)
#define parser_hash_viewGET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define parser_hash_view(...) parser_hash_viewGET_MACRO(__VA_ARGS__,parser_hash_viewf3,parser_hash_viewf2)(__VA_ARGS__)

#define parser_get_viewf1(a) native_parser_get_view(a)
#define parser_get_viewf2(a, b) struct native_view *a = native_parser_get_view(b)
#define parser_get_viewGET_MACRO(_1,_2,NAME,...) NAME
#define parser_get_view(...) parser_get_viewGET_MACRO(__VA_ARGS__,parser_get_viewf2,parser_get_viewf1)(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

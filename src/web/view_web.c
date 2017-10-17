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

#if OS == WEB
#include <emscripten.h>
#include <emscripten/emscripten.h>

#include <pthread.h>
#include <smartfox/data.h>
#include <cherry/memory.h>
#include <cherry/lock.h>
#include <cherry/list.h>
#include <cherry/stdio.h>
#include <cherry/bytes.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/math/math.h>
#include <native_ui/view.h>
#include <native_ui/view_layout_controller.h>
#include <native_ui/align.h>
#include <native_ui/parser.h>
#include <native_ui/touch.h>
#include <native_ui/manager.h>

static int __init__ = 0;
pthread_mutex_t __shared_nview_touch_lock;
static u8 __view_touched = 0;
static u8 __view_touched_move = 0;
static u8 __view_touched_end = 0;

EMSCRIPTEN_KEEPALIVE
void js_nview_touch_began(struct nview *v, float x, float y)
{
        if(__view_touched || !v) return;

        __view_touched = 1;
        struct nview_touch_data *data = nview_touch_data_alloc();
        list_add_tail(&data->head, &v->touch_list);
        list_add_tail(&data->shared_head, &nmanager_shared()->touches);
        data->view = v;
        data->type = NATIVE_UI_TOUCH_BEGAN;
        data->point = (union vec2){x, y};
        data->pscr = (union vec2){0, 0};

        // nview_touch_began(v, (union vec2){x, y});
}

EMSCRIPTEN_KEEPALIVE
void js_nview_touch_moved(struct nview *v, float x, float y)
{
        if(!__view_touched || !v) return;
        struct nview_touch_data *data = nview_touch_data_alloc();
        list_add_tail(&data->head, &v->touch_list);
        list_add_tail(&data->shared_head, &nmanager_shared()->touches);
        data->view = v;
        data->type = NATIVE_UI_TOUCH_MOVED;
        data->point = (union vec2){x, y};
        data->pscr = (union vec2){0, 0};

        // nview_touch_moved(v, (union vec2){x, y});
}

EMSCRIPTEN_KEEPALIVE
void js_nview_touch_ended(struct nview *v, float x, float y)
{
        if(!__view_touched || !v) return;

        if(__view_touched_end == 0) {
                __view_touched_end = 1;
                struct nview_touch_data *data = nview_touch_data_alloc();
                list_add_tail(&data->head, &v->touch_list);
                list_add_tail(&data->shared_head, &nmanager_shared()->touches);
                data->view = v;
                data->type = NATIVE_UI_TOUCH_ENDED;
                data->point = (union vec2){x, y};
                data->pscr = (union vec2){0, 0};
                // nview_touch_ended(v, (union vec2){x, y});
        }
        __view_touched_end = 0;
        __view_touched = 0;
}

EMSCRIPTEN_KEEPALIVE
void js_nview_touch_cancelled(struct nview *v, float x, float y)
{
        if(!__view_touched || !v) return;

        if(__view_touched_end == 0) {
                __view_touched_end = 1;
                struct nview_touch_data *data = nview_touch_data_alloc();
                list_add_tail(&data->head, &v->touch_list);
                list_add_tail(&data->shared_head, &nmanager_shared()->touches);
                data->view = v;
                data->type = NATIVE_UI_TOUCH_CANCELLED;
                data->point = (union vec2){x, y};
                data->pscr = (union vec2){0, 0};
                // nview_touch_cancelled(v, (union vec2){x, y});
        }
        __view_touched_end = 0;
        __view_touched = 0;
}

static void __clear()
{
        pthread_mutex_destroy(&__shared_nview_touch_lock);
}

static void __setup()
{
        if(!__init__) {
                __init__ = 1;
                cache_add(__clear);
                pthread_mutex_init(&__shared_nview_touch_lock, NULL);
        }
}

struct nview *nview_alloc()
{
        __setup();
        struct nview *p           = smalloc(sizeof(struct nview), nview_free);
        nview_init(p);

        p->ptr                    = 0;

        nview_show_view(p);
        return p;
}

void nview_free(struct nview *p)
{
        nview_free_common(p);
        /*
         * deallocate view content
         */
        if(p->ptr) {
                /*
                 * free js view
                 */
        }
        sfree(p);
}

static void __change_view(struct nview *p, i32 jsview)
{
        if(p->ptr) {
                EM_ASM_({
                        __shared_object_free($0);
                }, p->ptr);
        }

        p->ptr = jsview;

        if(p->parent && p->parent->ptr) {
                EM_ASM_({
                        var pr = __shared_object_get($0);
                        var ch = __shared_object_get($1);
                        view_helper_shared.view_add_child(pr, ch);
                }, p->parent->ptr, p->ptr);
        }
}

void nview_show_view(struct nview *p)
{
        p->type                 = NATIVE_UI_VIEW;

        i32 np = (i32) EM_ASM_({
                var obj = view_helper_shared.view_alloc();
                view_helper_shared.view_set_native_ptr(obj, $0);
                return obj.__soi__;
        }, p);

        __change_view(p, np);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void nview_show_image(struct nview *p, char *path)
{
        p->type                 = NATIVE_UI_IMAGE;
        i32 np = (i32) EM_ASM_({
                var obj = view_helper_shared.image_alloc();
                view_helper_shared.view_set_native_ptr(obj, $0);
                return obj.__soi__;
        }, p);

        __change_view(p, np);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);

        struct string *vpath = file_version_path(path);
        nview_on_change_imageview(p, vpath->ptr);
        string_free(vpath);
}

void nview_show_label(struct nview *p)
{
        p->type                 = NATIVE_UI_LABEL;
        i32 np = (i32) EM_ASM_({
                var obj = view_helper_shared.label_alloc();
                view_helper_shared.view_set_native_ptr(obj, $0);
                return obj.__soi__;
        }, p);

        __change_view(p, np);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);

        nview_on_change_label(p);
}

void nview_show_textfield(struct nview *p)
{
        debug("show text field\n");
        p->type                 = NATIVE_UI_TEXTFIELD;
        i32 np = (i32) EM_ASM_({
                var obj = view_helper_shared.textfield_alloc();
                view_helper_shared.view_set_native_ptr(obj, $0);
                return obj.__soi__;
        }, p);

        __change_view(p, np);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void nview_show_textview(struct nview *p)
{
        p->type                 = NATIVE_UI_TEXTVIEW;
        i32 np = (i32) EM_ASM_({
                var obj = view_helper_shared.textfield_alloc();
                view_helper_shared.view_set_native_ptr(obj, $0);
                return obj.__soi__;
        }, p);

        __change_view(p, np);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void nview_show_listview(struct nview *p)
{
        p->type                 = NATIVE_UI_LISTVIEW;
        i32 np = (i32) EM_ASM_({
                var obj = view_helper_shared.view_alloc();
                view_helper_shared.view_set_native_ptr(obj, $0);
                return obj.__soi__;
        }, p);

        __change_view(p, np);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);

        nview_on_change_listview(p);
}

void nview_add_child(struct nview *p, struct nview *c)
{
        if(c->parent && c->parent->name_to_child) {
                map_remove_key(c->parent->name_to_child, qskey(c->name));
        }

        list_del(&c->head);
        list_add_tail(&c->head, &p->children);
        c->parent = p;

        if(c->name->len) {
                if(!p->name_to_child) {
                        p->name_to_child = map_alloc(sizeof(struct nview *));
                        map_set(p->name_to_child, qskey(c->name), &c);
                }
        }

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_remove_from_parent(pr);
        }, c->ptr);

        EM_ASM_({
                var pr = __shared_object_get($0);
                var ch = __shared_object_get($1);
                view_helper_shared.view_add_child(pr, ch);
        }, p->ptr, c->ptr);

        nview_set_position(c, c->position);
        nview_set_size(c, c->size);
        nview_set_scale(c, c->scale);
        nview_set_rotation(c, c->rotation);
        nview_set_clip(c, c->clip);
        nview_set_color(c, c->color, c->border_color);
        nview_set_alpha(c, c->alpha);
        nview_set_anchor(c, c->anchor);
        nview_set_visible(c, c->visible);
        nview_set_user_interaction_enabled(c, c->user_interaction_enabled);
}

void nview_remove_from_parent(struct nview *p)
{
        if(!p->parent) return;

        if(p->parent && p->parent->name_to_child) {
                map_remove_key(p->parent->name_to_child, qskey(p->name));
        }

        list_del_init(&p->head);
        p->parent = NULL;

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_remove_from_parent(pr);
        }, p->ptr);
}

void nview_set_position(struct nview *p, union vec2 position)
{
        p->position     = position;

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_position(pr, $1, $2);
        }, p->ptr, position.x, position.y);
}

void nview_set_size(struct nview *p, union vec2 size)
{
        p->size         = size;
        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_size(pr, $1, $2);
        }, p->ptr, size.width, size.height);
}

void nview_set_scale(struct nview *p, union vec2 scale)
{
        p->scale                        = scale;
        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_scale(pr, $1, $2);
        }, p->ptr, scale.x, scale.y);
}

void nview_set_rotation(struct nview *p, union vec3 rotation)
{
        p->rotation                     = rotation;

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_rotation(pr, $1, $2, $3);
        }, p->ptr, rotation.x, rotation.y, rotation.z);
}

void nview_set_color(struct nview *p, union vec4 *color, union vec4 *border)
{
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

        if(p->color) {
                EM_ASM_({
                        var pr = __shared_object_get($0);
                        view_helper_shared.view_set_color(pr, $1, $2, $3, $4);
                }, p->ptr, color->r, color->g, color->b, color->a);
        } else {
                EM_ASM_({
                        var pr = __shared_object_get($0);
                        view_helper_shared.view_set_color(pr, $1, $2, $3, $4);
                }, p->ptr, (float)-1, (float)-1, (float)-1, (float)-1);
        }

        if(p->border_color) {
                EM_ASM_({
                        var pr = __shared_object_get($0);
                        view_helper_shared.view_set_border_color(pr, $1, $2, $3, $4);
                }, p->ptr, border->r, border->g, border->b, border->a);
        } else {
                EM_ASM_({
                        var pr = __shared_object_get($0);
                        view_helper_shared.view_set_border_color(pr, $1, $2, $3, $4);
                }, p->ptr, (float)-1, (float)-1, (float)-1, (float)-1);
        }
}

void nview_set_visible(struct nview *p, u8 visible)
{
        p->visible = visible;

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_visible(pr, $1);
        }, p->ptr, (int)visible);
}

void nview_set_alpha(struct nview *p, float alpha)
{
        p->alpha                        = alpha;

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_alpha(pr, $1);
        }, p->ptr, alpha);
}

void nview_set_clip(struct nview *p, u8 clip)
{
        p->clip                         = clip;

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_clip(pr, $1);
        }, p->ptr, clip);

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_border(pr, $1);
        }, p->ptr, (int)p->clip_rounds.x);
}

void nview_set_anchor(struct nview *p, union vec2 anchor)
{
        p->anchor                       = anchor;
        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_anchor(pr, $1, $2);
        }, p->ptr, anchor.x, anchor.y);
}

void nview_set_user_interaction_enabled(struct nview *p, u8 enabled)
{
        p->user_interaction_enabled     = enabled;

        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_user_interaction(pr, $1);
        }, p->ptr, (int)enabled);
}

union vec2 nview_convert_point_to_view(struct nview *from, union vec2 point, struct nview *to)
{
        i32 fleft = (i32) EM_ASM_({
                var pr = __shared_object_get($0);
                return view_helper_shared.view_get_offset_window_left(pr);
        }, from->ptr);

        i32 ftop = (i32) EM_ASM_({
                var pr = __shared_object_get($0);
                return view_helper_shared.view_get_offset_window_top(pr);
        }, from->ptr);

        i32 tleft = (i32) EM_ASM_({
                var pr = __shared_object_get($0);
                return view_helper_shared.view_get_offset_window_left(pr);
        }, to->ptr);

        i32 ttop = (i32) EM_ASM_({
                var pr = __shared_object_get($0);
                return view_helper_shared.view_get_offset_window_top(pr);
        }, to->ptr);

        return (union vec2){
                .x = point.x + tleft - fleft,
                .y = point.y + ttop - ftop
        };
}

/*
 * custom view functions
 */
struct string *nview_get_text(struct nview *p)
{
        if(p->type != NATIVE_UI_TEXTFIELD) return NULL;

        return NULL;
}

void nview_set_text_placeholder(struct nview *p, char *text, size_t len)
{
        if(p->type != NATIVE_UI_TEXTFIELD) return;

        // custom_text_field_set_placeholder(p->ptr, text, len);
}

void nview_on_change_label(struct nview *p)
{

}

void nview_update_label(struct nview *p)
{

}

void nview_set_text_align(struct nview *p, u8 text_align)
{

}

void nview_set_text_multiline(struct nview *p, u8 multiline)
{

}

void nview_set_text(struct nview *p, char *text, size_t len)
{

}

void nview_set_text_color(struct nview *p, union vec4 color)
{

}

void nview_set_font(struct nview *p, char *font_name, size_t len, size_t size)
{

}


void nview_on_change_imageview(struct nview *p, char *path)
{
        EM_ASM_({
                var pr = __shared_object_get($0);
                view_helper_shared.view_set_image(pr, Pointer_stringify($1));
        }, p->ptr, path);
}
#endif

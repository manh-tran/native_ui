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
#ifndef __NATIVE_UI_VIEW_H__
#define __NATIVE_UI_VIEW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

struct native_view *native_view_alloc();

void native_view_free(struct native_view *p);

void native_view_free_common(struct native_view *p);

void native_view_init(struct native_view *p);

void native_view_schedule(struct native_view *p, char *key, size_t len, int count, native_task_delegate delegate);

void native_view_unschedule(struct native_view *p, char *key, size_t len);

void native_view_set_clip(struct native_view *p, u8 clip);

struct native_parser *native_view_get_parser(struct native_view *p);

union vec2 native_view_get_screen_pos(struct native_view *p, union vec2 point);

/*
 * show views
 */
void native_view_show_view(struct native_view *p);

void native_view_show_image(struct native_view *p, char *path);

void native_view_show_label(struct native_view *p);

void native_view_show_textfield(struct native_view *p);

void native_view_show_textview(struct native_view *p);

void native_view_show_listview(struct native_view *p);

void native_view_on_change_listview(struct native_view *p);

void native_view_on_change_imageview(struct native_view *p, char *path);

void native_view_on_change_label(struct native_view *p);

void native_view_set_text(struct native_view *p, char *text, size_t len);

void native_view_set_font(struct native_view *p, char *font, size_t len, size_t size);

void native_view_set_text_align(struct native_view *p, u8 text_align);

void native_view_set_text_multiline(struct native_view *p, u8 multiline);

void native_view_set_text_color(struct native_view *p, union vec4 color);

void native_view_set_text_placeholder(struct native_view *p, char *text, size_t len);

struct string *native_view_get_text(struct native_view *p);

/*
 * parent child
 */
void native_view_add_child(struct native_view *p, struct native_view *c);

void native_view_remove_from_parent(struct native_view *p);

void native_view_remove_all_children(struct native_view *p);

/*
 * set transform
 */
void native_view_set_position(struct native_view *p, union vec2 position);

void native_view_set_size(struct native_view *p, union vec2 size);

void native_view_set_fixed_size(struct native_view *p, union vec2 size);

void native_view_set_scale(struct native_view *p, union vec2 scale);

void native_view_set_rotation(struct native_view *p, union vec3 rotation);

void native_view_set_anchor(struct native_view *p, union vec2 anchor);

void native_view_set_color(struct native_view *p, union vec4 *color, union vec4 *border);

void native_view_set_visible(struct native_view *p, u8 visible);

void native_view_set_alpha(struct native_view *p, float alpha);

void native_view_update_layout(struct native_view *p);

void native_view_request_layout(struct native_view *p);

void native_view_update_label(struct native_view *p);

void native_view_set_layout_type(struct native_view *p, u8 type);

void native_view_request_position(struct native_view *p, union vec2 position);

void native_view_request_size(struct native_view *p, union vec2 size);

void native_view_request_fixed_size(struct native_view *p, union vec2 size);

void native_view_request_scale(struct native_view *p, union vec2 scale);

void native_view_request_rotation(struct native_view *p, union vec3 rotation);

void native_view_request_margin(struct native_view *p, union vec4 margin);

void native_view_request_padding(struct native_view *p, union vec4 padding);

/*
 * touches
 */
void native_view_touch_began(struct native_view *p, union vec2 liv);

void native_view_touch_moved(struct native_view *p, union vec2 liv);

void native_view_touch_ended(struct native_view *p, union vec2 liv);

void native_view_touch_cancelled(struct native_view *p, union vec2 liv);

void native_view_text_done(struct native_view *p);

void native_view_set_user_interaction_enabled(struct native_view *p, u8 enabled);

void native_view_set_touch_data(struct native_view *p, void *touch_data, void(*touch_data_free)(void*));

void native_view_set_touch_began_delegate(struct native_view *p, native_controller_tbeganf delegate);

void native_view_set_touch_moved_delegate(struct native_view *p, native_controller_tmovedf delegate);

void native_view_set_touch_ended_delegate(struct native_view *p, native_controller_tendedf delegate);

void native_view_set_touch_cancelled_delegate(struct native_view *p, native_controller_tcancelledf delegate);

void native_view_set_text_done_delegate(struct native_view *p, native_controller_txtf delegate);

void native_view_set_intercept_horizontal(struct native_view *from, struct native_view *to);

void native_view_set_intercept_vertical(struct native_view *from, struct native_view *to);

union vec2 native_view_convert_point_to_view(struct native_view *from, union vec2 point, struct native_view *to);

/*
 * touch list view
 */
void native_view_list_view_touch_began(struct native_view *p, union vec2 liv);

void native_view_list_view_touch_moved(struct native_view *p, union vec2 liv);

void native_view_list_view_touch_ended(struct native_view *p, union vec2 liv);

void native_view_list_view_touch_cancelled(struct native_view *p, union vec2 liv);

/*
 * actions
 */
void native_view_clear_all_actions(struct native_view *p);

struct naction *native_view_margin_to(struct native_view *p, union vec4 v, float duration, u8 ease, i16 repeat);

struct naction *native_view_margin_by(struct native_view *p, union vec4 v, float duration, u8 ease, i16 repeat);

struct naction *native_view_move_to(struct native_view *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *native_view_move_by(struct native_view *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *native_view_scale_to(struct native_view *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *native_view_scale_by(struct native_view *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *native_view_rotate_by(struct native_view *p, union vec3 angle, float duration, u8 ease, i16 repeat);

struct naction *native_view_alpha_to(struct native_view *p, float v, float duration, u8 ease, i16 repeat);

struct naction *native_view_alpha_by(struct native_view *p, float v, float duration, u8 ease, i16 repeat);

struct naction *native_view_delay(struct native_view *, float duration);

struct naction *native_view_show(struct native_view *p);

struct naction *native_view_hide(struct native_view *p);

struct naction *native_view_callback(struct native_view *, nactionf callback, void *callback_data);

void native_view_run_action(struct native_view *p, ...);

#ifdef __cplusplus
}
#endif

#endif

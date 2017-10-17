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

struct nview *nview_alloc();

void nview_free(struct nview *p);

void nview_free_common(struct nview *p);

void nview_init(struct nview *p);

void nview_schedule(struct nview *p, char *key, size_t len, int count, ntaskf delegate);

void nview_unschedule(struct nview *p, char *key, size_t len);

void nview_set_clip(struct nview *p, u8 clip);

struct nparser *nview_get_parser(struct nview *p);

union vec2 nview_get_screen_pos(struct nview *p, union vec2 point);

/*
 * show views
 */
void nview_show_view(struct nview *p);

void nview_show_image(struct nview *p, char *path);

void nview_show_label(struct nview *p);

void nview_show_textfield(struct nview *p);

void nview_show_textview(struct nview *p);

void nview_show_listview(struct nview *p);

void nview_on_change_listview(struct nview *p);

void nview_on_change_imageview(struct nview *p, char *path);

void nview_on_change_label(struct nview *p);

void nview_set_text(struct nview *p, char *text, size_t len);

void nview_set_font(struct nview *p, char *font, size_t len, size_t size);

void nview_set_text_align(struct nview *p, u8 text_align);

void nview_set_text_multiline(struct nview *p, u8 multiline);

void nview_set_text_color(struct nview *p, union vec4 color);

void nview_set_text_placeholder(struct nview *p, char *text, size_t len);

struct string *nview_get_text(struct nview *p);

/*
 * parent child
 */
void nview_add_child(struct nview *p, struct nview *c);

void nview_remove_from_parent(struct nview *p);

void nview_remove_all_children(struct nview *p);

/*
 * set transform
 */
void nview_set_position(struct nview *p, union vec2 position);

void nview_set_size(struct nview *p, union vec2 size);

void nview_set_fixed_size(struct nview *p, union vec2 size);

void nview_set_scale(struct nview *p, union vec2 scale);

void nview_set_rotation(struct nview *p, union vec3 rotation);

void nview_set_anchor(struct nview *p, union vec2 anchor);

void nview_set_color(struct nview *p, union vec4 *color, union vec4 *border);

void nview_set_visible(struct nview *p, u8 visible);

void nview_set_alpha(struct nview *p, float alpha);

void nview_update_layout(struct nview *p);

void nview_request_layout(struct nview *p);

void nview_update_label(struct nview *p);

void nview_set_layout_type(struct nview *p, u8 type);

void nview_request_position(struct nview *p, union vec2 position);

void nview_request_size(struct nview *p, union vec2 size);

void nview_request_fixed_size(struct nview *p, union vec2 size);

void nview_request_scale(struct nview *p, union vec2 scale);

void nview_request_rotation(struct nview *p, union vec3 rotation);

void nview_request_margin(struct nview *p, union vec4 margin);

void nview_request_padding(struct nview *p, union vec4 padding);

/*
 * touches
 */
void nview_touch_began(struct nview *p, union vec2 liv);

void nview_touch_moved(struct nview *p, union vec2 liv);

void nview_touch_ended(struct nview *p, union vec2 liv);

void nview_touch_cancelled(struct nview *p, union vec2 liv);

void nview_text_done(struct nview *p);

void nview_set_user_interaction_enabled(struct nview *p, u8 enabled);

void nview_set_touch_began_delegate(struct nview *p, nexec_tbeganf delegate);

void nview_set_touch_moved_delegate(struct nview *p, nexec_tmovedf delegate);

void nview_set_touch_ended_delegate(struct nview *p, nexec_tendedf delegate);

void nview_set_touch_cancelled_delegate(struct nview *p, nexec_tcancelledf delegate);

void nview_set_text_done_delegate(struct nview *p, nexec_txtf delegate);

void nview_set_intercept_horizontal(struct nview *from, struct nview *to);

void nview_set_intercept_vertical(struct nview *from, struct nview *to);

union vec2 nview_convert_point_to_view(struct nview *from, union vec2 point, struct nview *to);

/*
 * touch list view
 */
void nview_list_view_touch_began(struct nview *p, union vec2 liv);

void nview_list_view_touch_moved(struct nview *p, union vec2 liv);

void nview_list_view_touch_ended(struct nview *p, union vec2 liv);

void nview_list_view_touch_cancelled(struct nview *p, union vec2 liv);

/*
 * actions
 */
void nview_clear_all_actions(struct nview *p);

struct naction *nview_margin_to(struct nview *p, union vec4 v, float duration, u8 ease, i16 repeat);

struct naction *nview_margin_by(struct nview *p, union vec4 v, float duration, u8 ease, i16 repeat);

struct naction *nview_move_to(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *nview_move_by(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *nview_scale_to(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *nview_scale_by(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat);

struct naction *nview_rotate_by(struct nview *p, union vec3 angle, float duration, u8 ease, i16 repeat);

struct naction *nview_alpha_to(struct nview *p, float v, float duration, u8 ease, i16 repeat);

struct naction *nview_alpha_by(struct nview *p, float v, float duration, u8 ease, i16 repeat);

struct naction *nview_delay(struct nview *, float duration);

struct naction *nview_show(struct nview *p);

struct naction *nview_hide(struct nview *p);

struct naction *nview_callback(struct nview *, nactionf callback, void *callback_data);

void nview_run_action(struct nview *p, ...);

#ifdef __cplusplus
}
#endif

#endif

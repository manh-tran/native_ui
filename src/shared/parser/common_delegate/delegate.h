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
#ifndef __NATIVE_UI_PARSER_COMMON_DELEGATE_H__
#define __NATIVE_UI_PARSER_COMMON_DELEGATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>
#include <cherry/xml/types.h>

void parse_common_width(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_height(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_type(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_align(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_x(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_y(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_scale(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_rotation_x(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_rotation_y(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_rotation_z(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_anchor_x(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_anchor_y(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_touch(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_clip(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_round(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_color(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_border(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_alpha(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_margin_top(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_margin_left(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_margin_bottom(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_margin_right(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_padding_top(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_padding_left(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_padding_bottom(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_padding_right(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_hash(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_touch_delegate(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_intercept_horizontal(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_intercept_vertical(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_visible(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent);

void parse_common_weights(struct native_view *v, struct xml_attribute *a, struct native_parser *p, struct native_parser *parent);

void parse_common_weight(struct native_view *v, struct xml_attribute *a, struct native_parser *p, struct native_parser *parent);

#ifdef __cplusplus
}
#endif

#endif

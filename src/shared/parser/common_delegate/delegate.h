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

void parse_common_width(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_height(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_type(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_align(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_x(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_y(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_scale(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_rotation_x(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_rotation_y(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_rotation_z(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_anchor_x(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_anchor_y(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_touch(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_clip(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_round(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_color(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_border(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_alpha(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_margin_top(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_margin_left(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_margin_bottom(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_margin_right(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_padding_top(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_padding_left(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_padding_bottom(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_padding_right(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_hash(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_touch_delegate(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_intercept_horizontal(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_intercept_vertical(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);

void parse_common_visible(struct nview *v, struct xml_attribute *e, struct nparser *p, struct nparser *parent);


#ifdef __cplusplus
}
#endif

#endif

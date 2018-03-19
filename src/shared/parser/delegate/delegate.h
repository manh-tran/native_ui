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
#ifndef __NATIVE_UI_PARSER_DELEGATE_H__
#define __NATIVE_UI_PARSER_DELEGATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>
#include <cherry/xml/types.h>

struct native_view *parser_view(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_label(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_textfield(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_textview(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_listview(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_pageview(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_image(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_label(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_link(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_touch(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_controller(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

struct native_view *parser_template(struct xml_element *e, struct native_parser *parser, struct native_parser *parent);

#ifdef __cplusplus
}
#endif

#endif

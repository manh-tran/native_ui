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
#ifndef __NATIVE_UI_PARSER_H__
#define __NATIVE_UI_PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>
#include <cherry/xml/types.h>

struct native_parser *native_parser_alloc();

void native_parser_free(struct native_parser *p);

void native_parser_parse_file(struct native_parser *p, char *file, struct native_parser *parent);

void native_parser_parse_template(struct native_parser *p, struct native_parser *parent, char *key, size_t len);

struct native_view *native_parser_get_view(struct native_parser *p);

struct native_view *native_parser_get_hash_view(struct native_parser *p, char *key, size_t len);

struct native_touch *native_parser_get_touch(struct native_parser *p, char *key, size_t len);

#ifdef __cplusplus
}
#endif

#endif

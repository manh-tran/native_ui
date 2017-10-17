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

struct nparser *nparser_alloc();

void nparser_free(struct nparser *p);

void nparser_parse_file(struct nparser *p, char *file, struct nparser *parent);

void nparser_parse_template(struct nparser *p, struct nparser *parent, char *key, size_t len);

struct nview *nparser_get_view(struct nparser *p);

struct nview *nparser_get_hash_view(struct nparser *p, char *key, size_t len);

struct ntouch *nparser_get_touch(struct nparser *p, char *key, size_t len);

#ifdef __cplusplus
}
#endif

#endif

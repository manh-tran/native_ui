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
#include "delegate.h"
#include <native_ui/parser.h>
#include <native_ui/view.h>
#include <native_ui/view_layout_controller.h>
#include <native_ui/align.h>
#include <native_ui/touch_handle.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>
#include <cherry/list.h>
#include <cherry/xml/xml.h>

struct nview *parser_touch(struct xml_element *e, struct nparser *parser, struct nparser *parent)
{
        struct xml_attribute *key = xml_find_attribute(e, "key");
        if(key) {
                struct ntouch *handle = ntouch_alloc();

                if(map_has_key(parser->hash_touches, qskey(key->value))) {
                        struct ntouch *prev = map_get(parser->hash_touches, struct ntouch *, qskey(key->value));
                        ntouch_free(prev);
                        map_remove_key(parser->hash_touches, qskey(key->value));
                }
                map_set(parser->hash_touches, qskey(key->value), &handle);
        }

        return NULL;
}

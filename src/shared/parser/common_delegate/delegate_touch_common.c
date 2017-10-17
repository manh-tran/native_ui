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
#include <cherry/list.h>
#include <cherry/xml/xml.h>

void parse_common_touch(struct nview *v, struct xml_attribute *a, struct nparser *p, struct nparser *parent)
{
        if(strcmp(a->value->ptr, "true") == 0
        	|| strcmp(a->value->ptr, "TRUE") == 0
        	|| strcmp(a->value->ptr, "YES") == 0) {
        	nview_set_user_interaction_enabled(v, 1);
        } else {
        	nview_set_user_interaction_enabled(v, 0);
        }
}

void parse_common_touch_delegate(struct nview *v, struct xml_attribute *a, struct nparser *p, struct nparser *parent)
{
        struct nparser *root = p;
        while(root) {
                struct ntouch *handle = map_get(root->hash_touches,
                        struct ntouch *, qskey(a->value));
                if(handle) {
                        v->parser_touch_handle = handle;
                        break;
                }
                root = root->parent;
        }
        //
        // if(!handle) {
        //         handle = map_get(parent->hash_touches,
        //                 struct ntouch *, qskey(a->value));
        // }
        // if(handle) v->parser_touch_handle = handle;
}

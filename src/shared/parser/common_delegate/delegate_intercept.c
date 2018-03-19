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
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/xml/xml.h>
#include <cherry/stdio.h>

void parse_common_intercept_horizontal(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent)
{
        struct native_parser *root = p;
        while(root) {
                struct native_view *to = native_parser_get_hash_view(root, qskey(e->value));
                if(to) {
                        native_view_set_intercept_horizontal(v, to);
                        break;
                }
                root = root->parent;
        }

}

void parse_common_intercept_vertical(struct native_view *v, struct xml_attribute *e, struct native_parser *p, struct native_parser *parent)
{
        struct native_parser *root = p;
        while(root) {
                struct native_view *to = native_parser_get_hash_view(root, qskey(e->value));
                if(to) {
                        native_view_set_intercept_vertical(v, to);
                        break;
                }
                root = root->parent;
        }

        // struct native_view *to = native_parser_get_hash_view(p, qskey(e->value));
        // if(to) {
        //         native_view_set_intercept_vertical(v, to);
        // }
}

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

struct native_view *parser_image(struct xml_element *e, struct native_parser *parser, struct native_parser *parent)
{
        char *src = NULL;

        struct list_head *attr_head;
        list_for_each(attr_head, &e->attributes) {
                struct xml_attribute *a = (struct xml_attribute *)
                        ((char *)attr_head - offsetof(struct xml_attribute, head));
                if(strcmp(a->name->ptr, "src") == 0) {
                        src = a->value->ptr;
                }
        }

        struct native_view *p = native_view_alloc();
        native_view_show_image(p, src ? src : "");
        native_view_set_clip(p, 1);
        return p;
}

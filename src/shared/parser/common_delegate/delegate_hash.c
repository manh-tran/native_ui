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
#include <cherry/stdio.h>
#include <cherry/list.h>
#include <cherry/xml/xml.h>

void parse_common_hash(struct native_view *v, struct xml_attribute *a, struct native_parser *p, struct native_parser *parent)
{
        if(map_has_key(p->hash_views, qskey(a->value))) {
                struct list_head *head = map_get(p->hash_views, struct list_head *, qskey(a->value));
                /*
                 * remove previous hash link but keep old view
                 */
                list_del_init(head);
        } else {
                struct list_head *head = smalloc(sizeof(struct list_head), NULL);
                INIT_LIST_HEAD(head);
                map_set(p->hash_views, qskey(a->value), &head);
        }
        struct list_head *head = map_get(p->hash_views, struct list_head *, qskey(a->value));
        list_del(&v->hash_head);
        list_add_tail(&v->hash_head, head);
}

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
#include <native_ui/view_controller.h>
#include <native_ui/align.h>
#include <native_ui/touch_handle.h>
#include <native_ui/file.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/xml/xml.h>

static void __parse_bind_touch(struct nparser *parent, struct nparser *child, struct xml_element *e, struct nparser *parent_parser)
{
        struct xml_attribute *bind_touch        = xml_find_attribute(e, "bind_touch");
        if(!bind_touch) return;

        struct array *pairs                     = array_alloc(sizeof(struct string *), ORDERED);
        string_split(bind_touch->value, ";", pairs);

        struct string **pair;
        array_for_each(pair, pairs) {
                struct array *names             = array_alloc(sizeof(struct string *), ORDERED);
                string_split((*pair), "=", names);

                if(names->len == 2) {
                        struct string *first = array_get(names, struct string *, 0);
                        struct string *second = array_get(names, struct string *, 1);

                        //struct ntouch *parent_handle = nparser_get_touch(parent, qskey(second));
                        struct ntouch *child_handle = nparser_get_touch(child, qskey(first));

                        struct nparser *root = parent;
                        while(root) {
                                struct ntouch *parent_handle = nparser_get_touch(root, qskey(second));
                                if(parent_handle && child_handle) {
                                        ntouch_link(parent_handle, child_handle);
                                        break;
                                }
                                root = root->parent;
                        }

                        // if(parent_handle && child_handle) {
                        //         ntouch_link(parent_handle, child_handle);
                        // } else if(parent_parser){
                        //         parent_handle = nparser_get_touch(parent_parser, qskey(second));
                        //
                        //         if(parent_handle && child_handle) {
                        //                 ntouch_link(parent_handle, child_handle);
                        //         }
                        // }
                }

                array_deep_free(names, struct string *, string_free);
        }

        array_deep_free(pairs, struct string *, string_free);
}

struct nview *parser_link(struct xml_element *e, struct nparser *parser, struct nparser *parent)
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
        if(src) {
                struct nparser *sub_parser = nparser_alloc();
                nparser_parse_file(sub_parser, src, parser);

                __parse_bind_touch(parser, sub_parser, e, parent);

                if(parser->controller && sub_parser->controller) {
                        nexec_link(parser->controller, sub_parser->controller);
                }

                struct nview *view = (struct nview *)
                        ((char *)sub_parser->view.next - offsetof(struct nview, parser));

                return view;
        } else {
                return nview_alloc();
        }
}

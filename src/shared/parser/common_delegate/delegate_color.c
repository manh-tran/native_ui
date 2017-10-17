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

void parse_common_color(struct nview *v, struct xml_attribute *a, struct nparser *p, struct nparser *parent)
{
        if(a->value->len && a->value->ptr[0] == '#') {
                if(a->value->len == 7) {
                        char buf[3];
                        buf[2] = '\0';
                        buf[0] = a->value->ptr[1];
                        buf[1] = a->value->ptr[2];
                        int r = strtol(buf, NULL, 16);
                        buf[0] = a->value->ptr[3];
                        buf[1] = a->value->ptr[4];
                        int g = strtol(buf, NULL, 16);
                        buf[0] = a->value->ptr[5];
                        buf[1] = a->value->ptr[6];
                        int b = strtol(buf, NULL, 16);
                        nview_set_color(v, &(union vec4){
                                r / 255.0f,
                                g / 255.0f,
                                b / 255.0f,
                                1.0f
                        }, v->border_color);
                } else if(a->value->len == 9) {
                        char buf[3];
                        buf[2] = '\0';
                        buf[0] = a->value->ptr[1];
                        buf[1] = a->value->ptr[2];
                        int r = strtol(buf, NULL, 16);
                        buf[0] = a->value->ptr[3];
                        buf[1] = a->value->ptr[4];
                        int g = strtol(buf, NULL, 16);
                        buf[0] = a->value->ptr[5];
                        buf[1] = a->value->ptr[6];
                        int b = strtol(buf, NULL, 16);
                        buf[0] = a->value->ptr[7];
                        buf[1] = a->value->ptr[8];
                        int a = strtol(buf, NULL, 16);
                        nview_set_color(v, &(union vec4){
                                r / 255.0f,
                                g / 255.0f,
                                b / 255.0f,
                                a / 255.0f
                        }, v->border_color);
                }
        }
}

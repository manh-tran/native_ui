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
#include <cherry/stdio.h>
#include <cherry/xml/xml.h>

static union vec4 parse_color(struct string *p)
{
        /*
         * set label default color to black
         */
        union vec4 result = (union vec4){0, 0, 0, 1};
        if(p->len && p->ptr[0] == '#') {
                if(p->len == 7) {
                        char buf[3];
                        buf[2] = '\0';
                        buf[0] = p->ptr[1];
                        buf[1] = p->ptr[2];
                        int r = strtol(buf, NULL, 16);
                        buf[0] = p->ptr[3];
                        buf[1] = p->ptr[4];
                        int g = strtol(buf, NULL, 16);
                        buf[0] = p->ptr[5];
                        buf[1] = p->ptr[6];
                        int b = strtol(buf, NULL, 16);

                        result = (union vec4){
                                r / 255.0f,
                                g / 255.0f,
                                b / 255.0f,
                                1.0f
                        };
                } else if(p->len == 9) {
                        char buf[3];
                        buf[2] = '\0';
                        buf[0] = p->ptr[1];
                        buf[1] = p->ptr[2];
                        int r = strtol(buf, NULL, 16);
                        buf[0] = p->ptr[3];
                        buf[1] = p->ptr[4];
                        int g = strtol(buf, NULL, 16);
                        buf[0] = p->ptr[5];
                        buf[1] = p->ptr[6];
                        int b = strtol(buf, NULL, 16);
                        buf[0] = p->ptr[7];
                        buf[1] = p->ptr[8];
                        int a = strtol(buf, NULL, 16);

                        result = (union vec4){
                                r / 255.0f,
                                g / 255.0f,
                                b / 255.0f,
                                a / 255.0f
                        };
                }
        }
        return result;
}

struct native_view *parser_label(struct xml_element *e, struct native_parser *parser, struct native_parser *parent)
{
    struct native_view *p = native_view_alloc();

    native_view_show_label(p);
    native_view_set_text(p, qskey(e->value));

    struct xml_attribute *font = xml_find_attribute(e, "font");
    struct xml_attribute *font_size = xml_find_attribute(e, "font_size");
    struct xml_attribute *text_align = xml_find_attribute(e, "text_align");
    struct xml_attribute *multiline = xml_find_attribute(e, "multiline");
    struct xml_attribute *text_color = xml_find_attribute(e, "text_color");

    if(font && font_size) {
            float size = (float)satof(font_size->value->ptr);
            native_view_set_font(p, qskey(font->value), size);
    } else if(font) {
            native_view_set_font(p, qskey(font->value), 14);
    }

    if(text_align) {
            if(strcmp(text_align->value->ptr, "left") == 0 || strcmp(text_align->value->ptr, "LEFT") == 0) {
                    native_view_set_text_align(p, NATIVE_UI_LABEL_ALIGN_LEFT);
            } else if(strcmp(text_align->value->ptr, "right") == 0 || strcmp(text_align->value->ptr, "RIGHT") == 0) {
                    native_view_set_text_align(p, NATIVE_UI_LABEL_ALIGN_RIGHT);
            } else if(strcmp(text_align->value->ptr, "center") == 0 || strcmp(text_align->value->ptr, "CENTER") == 0) {
                    native_view_set_text_align(p, NATIVE_UI_LABEL_ALIGN_CENTER);
            } else if(strcmp(text_align->value->ptr, "justified") == 0 || strcmp(text_align->value->ptr, "JUSTIFIED") == 0) {
                    native_view_set_text_align(p, NATIVE_UI_LABEL_ALIGN_JUSTIFIED);
            }
    }

    if(multiline) {
            if(strcmp(multiline->value->ptr, "true") == 0
                || strcmp(multiline->value->ptr, "TRUE") == 0
                || strcmp(multiline->value->ptr, "yes") == 0
                || strcmp(multiline->value->ptr, "YES") == 0) {
                    native_view_set_text_multiline(p, 1);
            }
    }

    if(text_color) {
            native_view_set_text_color(p, parse_color(text_color->value));
    }

    return p;
}

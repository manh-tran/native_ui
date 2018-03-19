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

void parse_common_align(struct native_view *v, struct xml_attribute *a, struct native_parser *p, struct native_parser *parent)
{
        struct array *split = array_alloc(sizeof(struct string *), ORDERED);
        string_split(a->value, "|", split);
        struct string **align;
        array_for_each(align, split) {
                if(strcmp((*align)->ptr, "TOP") == 0 || strcmp((*align)->ptr, "top") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_TOP;
                } else if(strcmp((*align)->ptr, "RIGHT") == 0 || strcmp((*align)->ptr, "right") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_RIGHT;
                } else if(strcmp((*align)->ptr, "BOTTOM") == 0 || strcmp((*align)->ptr, "bottom") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_BOTTOM;
                } else if(strcmp((*align)->ptr, "LEFT") == 0 || strcmp((*align)->ptr, "left") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_LEFT;
                } else if(strcmp((*align)->ptr, "CENTER") == 0 || strcmp((*align)->ptr, "center") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_CENTER;
                } else if(strcmp((*align)->ptr, "OUT_TOP") == 0 || strcmp((*align)->ptr, "out_top") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_OUT_TOP;
                } else if(strcmp((*align)->ptr, "OUT_RIGHT") == 0 || strcmp((*align)->ptr, "out_right") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_OUT_RIGHT;
                } else if(strcmp((*align)->ptr, "OUT_BOTTOM") == 0 || strcmp((*align)->ptr, "out_bottom") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_OUT_BOTTOM;
                } else if(strcmp((*align)->ptr, "OUT_LEFT") == 0 || strcmp((*align)->ptr, "out_left") == 0) {
                        v->align->type |= NATIVE_UI_ALIGN_OUT_LEFT;
                }
        }

        array_deep_free(split, struct string *, string_free);
}

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
#include <cherry/stdlib.h>
#include <cherry/stdio.h>

void parse_common_width(struct native_view *v, struct xml_attribute *a, struct native_parser *p, struct native_parser *parent)
{
        if(strcmp(a->value->ptr, "match_parent") == 0) {
                v->align->size_width = NATIVE_UI_SIZE_MATCH_PARENT;
        } else if(strcmp(a->value->ptr, "fill_parent") == 0) {
                v->align->size_width = NATIVE_UI_SIZE_FILL;
        } else if(strcmp(a->value->ptr, "wrap_content") == 0) {
                v->align->size_width = NATIVE_UI_SIZE_WRAP_CONTENT;
        } else if(strcmp(a->value->ptr, "equal") == 0) {
                v->align->size_width = NATIVE_UI_SIZE_EQUAL;
        } else if(strcmp(a->value->ptr, "weight") == 0) {
                v->align->size_width = NATIVE_UI_SIZE_WEIGHT;
        } else {
                v->align->fixed_width = (float)satof(a->value->ptr);
                v->align->size_width = NATIVE_UI_SIZE_FIXED;
        }
}

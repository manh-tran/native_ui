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
#include <cherry/platform.h>

#if OS == DROID

#include <native_ui/view.h>
#include <native_ui/view_layout_controller.h>
#include <native_ui/align.h>
#include <native_ui/parser.h>
#include <native_ui/manager.h>
#include <native_ui/image.h>
#include <native_ui/data.h>
#include <native_ui/font.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>
#include "custom/textfield.h"

struct string *native_view_get_text(struct native_view *p)
{
        if(p->type != NATIVE_UI_TEXTFIELD) return NULL;

        return custom_text_field_get_text(p->ptr);
}

void native_view_set_text_placeholder(struct native_view *p, char *text, size_t len)
{
        if(p->type != NATIVE_UI_TEXTFIELD) return;

        custom_text_field_set_placeholder(p->ptr, text, len);
}


#endif

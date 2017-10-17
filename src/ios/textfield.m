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
#import <cherry/platform.h>

#if OS == IOS

#import <native_ui/view.h>
#import <native_ui/view_layout_controller.h>
#import <native_ui/align.h>
#import <native_ui/parser.h>
#import <native_ui/manager.h>
#import <native_ui/image.h>
#import <native_ui/data.h>
#import <native_ui/font.h>
#import <cherry/list.h>
#import <cherry/string.h>
#import <cherry/memory.h>
#import <cherry/array.h>
#import <cherry/map.h>
#import <cherry/stdio.h>
#import <cherry/math/math.h>
#import "custom/textfield.h"

struct string *nview_get_text(struct nview *p)
{
        if(p->type != NATIVE_UI_TEXTFIELD) return NULL;

        CustomTextField *view                       = (__bridge id)(p->ptr);

        if(view.text != nil) {
                return string_alloc_chars([view.text UTF8String], view.text.length);
        } else {
                return string_alloc(0);
        }
}

void nview_set_text_placeholder(struct nview *p, char *text, size_t len)
{
        if(p->type != NATIVE_UI_TEXTFIELD) return NULL;

        CustomTextField *view                       = (__bridge id)(p->ptr);

        [view setPlaceholder:[NSString stringWithUTF8String:text]];
}

#endif

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
#import "custom/label.h"

void native_view_on_change_label(struct native_view *p)
{
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
                p->custom_data          = NULL;
                p->custom_data_free     = NULL;
        }

        p->custom_data                          = native_view_label_data_alloc();
        p->custom_data_free                     = native_view_label_data_free;

        struct native_view_label_data *data     = (struct native_view_label_data *)p->custom_data;
        data->font                              = native_ui_font_get(qlkey("arial"), 14);

        CustomLabel *view                       = (__bridge id)(p->ptr);
        UIFont *font                            = (__bridge id)(data->font->ptr);
        [view setFont:font];
        view.textColor                          = [UIColor blackColor];
        view.numberOfLines                      = data->multi_line ? 0 : 1;

        native_view_update_label(p);
        native_view_set_text_align(p, NATIVE_UI_LABEL_ALIGN_LEFT);
}

void native_view_update_label(struct native_view *p)
{
        if(p->type != NATIVE_UI_LABEL) return;

        struct native_view_label_data *data     = (struct native_view_label_data *)p->custom_data;

        if(data->dirty || fabsf(p->size.width - data->current_width) > 1) {
                CustomLabel *view                       = (__bridge id)(p->ptr);

                view.bounds = CGRectMake(0, 0, p->size.width, 0);
                [view sizeToFit];

                union vec2 size;
                size.width      = p->size.width;
                size.height     = view.bounds.size.height;

                native_view_set_size(p, size);

                p->align->fixed_width                   = view.bounds.size.width;
                p->align->fixed_height                  = view.bounds.size.height;

                data->dirty                             = 0;
                data->current_width                     = p->size.width;
        }
}

void native_view_set_text_align(struct native_view *p, u8 text_align)
{
        if(p->type != NATIVE_UI_LABEL) return;

        struct native_view_label_data *data     = (struct native_view_label_data *)p->custom_data;
        data->align                             = text_align;
        data->dirty                             = 1;

        CustomLabel *view                       = (__bridge id)(p->ptr);

        switch (text_align) {
                case NATIVE_UI_LABEL_ALIGN_LEFT:
                        view.textAlignment                      = NSTextAlignmentLeft;
                        break;
                case NATIVE_UI_LABEL_ALIGN_RIGHT:
                        view.textAlignment                      = NSTextAlignmentRight;
                        break;
                case NATIVE_UI_LABEL_ALIGN_CENTER:
                        view.textAlignment                      = NSTextAlignmentCenter;
                        break;
                case NATIVE_UI_LABEL_ALIGN_JUSTIFIED:
                        view.textAlignment                      = NSTextAlignmentJustified;
                        break;
                default:
                        break;
        }
}

void native_view_set_text_multiline(struct native_view *p, u8 multiline)
{
        if(p->type != NATIVE_UI_LABEL) return;

        struct native_view_label_data *data     = (struct native_view_label_data *)p->custom_data;
        data->multi_line                        = multiline;
        data->dirty                             = 1;

        CustomLabel *view                       = (__bridge id)(p->ptr);
        view.numberOfLines                      = data->multi_line ? 0 : 1;

        native_view_request_layout(p);
}

void native_view_set_text(struct native_view *p, char *text, size_t len)
{
        if(p->type != NATIVE_UI_LABEL) return;

        struct native_view_label_data *data     = (struct native_view_label_data *)p->custom_data;
        data->dirty                             = 1;
        data->text->len                         = 0;
        string_cat(data->text, text, len);

        CustomLabel *view                       = (__bridge id)(p->ptr);
        [view setText:[NSString stringWithUTF8String:data->text->ptr]];

        native_view_request_layout(p);
}

void native_view_set_text_color(struct native_view *p, union vec4 color)
{
        if(p->type != NATIVE_UI_LABEL) return;

        struct native_view_label_data *data     = (struct native_view_label_data *)p->custom_data;
        data->color                             = color;

        CustomLabel *view                       = (__bridge id)(p->ptr);
        view.textColor                          = [UIColor colorWithRed:color.r green:color.g blue:color.b alpha:color.a];
}

void native_view_set_font(struct native_view *p, char *font_name, size_t len, size_t size)
{
        if(p->type != NATIVE_UI_LABEL) return;

        struct native_view_label_data *data     = (struct native_view_label_data *)p->custom_data;
        data->font                              = native_ui_font_get(font_name, len, size);
        data->dirty                             = 1;

        CustomLabel *view                       = (__bridge id)(p->ptr);
        UIFont *font                            = (__bridge id)(data->font->ptr);
        [view setFont:font];

        native_view_request_layout(p);
}

#endif

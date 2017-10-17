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

#import <native_ui/font.h>
#import <UIKit/UIKit.h>

void native_ui_font_init(struct native_ui_font *p)
{
        UIFont *font    =  [UIFont fontWithName:[NSString stringWithUTF8String:p->name->ptr] size:p->size];
        p->ptr          =  CFBridgingRetain(font);
}

void native_ui_font_free_platform(struct native_ui_font *p)
{
        if(p->ptr) {
                CFRelease(p->ptr);
                CFRelease(p->ptr);
                p->ptr                  = NULL;
        }
}

#endif

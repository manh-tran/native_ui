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

#import <native_ui/image.h>
#import <native_ui/file.h>
#import <cherry/memory.h>
#import <cherry/list.h>
#import <cherry/array.h>
#import <cherry/stdio.h>
#import <cherry/map.h>
#import <cherry/string.h>
#import <UIKit/UIKit.h>

void nimage_init(struct nimage *p, char *name)
{

        struct string *path     = nfile_fullpath(name);

        UIImage *img            = [UIImage imageWithContentsOfFile:[NSString stringWithUTF8String:path->ptr]];
        p->ptr                  = CFBridgingRetain(img);

        string_free(path);
}

void nimage_free(struct nimage *p)
{
        nimage_free_common(p);
        if(p->ptr) {
                CFRelease(p->ptr);
                // CFRelease(p->ptr);
                /*
                 * UIImage only need to call CFRelease one time
                 */
                p->ptr                  = NULL;
        }
        sfree(p);
}

#endif

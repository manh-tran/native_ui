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
#import <cherry/list.h>
#import <cherry/string.h>
#import <cherry/memory.h>
#import <cherry/array.h>
#import <cherry/map.h>
#import <cherry/stdio.h>
#import <cherry/math/math.h>
#import "custom/imageview.h"

void nview_on_change_imageview(struct nview *p, char *path)
{
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
                p->custom_data          = NULL;
                p->custom_data_free     = NULL;
        }

        p->custom_data                                  = image_view_data_alloc(path);
        p->custom_data_free                             = image_view_data_free;

        CustomImageView *view                           = (__bridge id)(p->ptr);

        struct nview_image_view_data *data        = (struct nview_image_view_data *)p->custom_data;
        struct nimage *image                   = nimage_get_image(&data->image_link);

        UIImage *im                                     = (__bridge id)(image->ptr);
        [view setImage:im];
        // view.image                                      = im;
}

#endif

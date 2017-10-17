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
#include <native_ui/data.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/math/math.h>
#include <cherry/list.h>
#include <native_ui/image.h>

struct nview_image_view_data *image_view_data_alloc(char *path)
{
        struct nview_image_view_data *p   = smalloc(sizeof(struct nview_image_view_data), image_view_data_free);
        INIT_LIST_HEAD(&p->image_link);
        p->path                                 = string_alloc_chars(path, strlen(path));
        nimage_link_path(path, &p->image_link);
        return p;
}

void image_view_data_free(struct nview_image_view_data *p)
{
        nimage_un_link(&p->image_link);
        string_free(p->path);
        sfree(p);
}

struct nview_list_view_data *nview_list_view_data_alloc()
{
        struct nview_list_view_data *p    = smalloc(sizeof(struct nview_list_view_data), nview_list_view_data_free);
        p->bouncing                             = 0;
        p->hold                                 = 0;
        p->velocity                             = (union vec2){0, 0};
        p->acceleration                         = (union vec2){0, 0};
        p->last_moved_point                     = (union vec2){0, 0};
        p->stop                                 = 0;
        p->scrolling                            = 0;

        return p;
}

void nview_list_view_data_free(struct nview_list_view_data *p)
{
        sfree(p);
}

struct nview_label_data *nview_label_data_alloc()
{
        struct nview_label_data *p        = smalloc(sizeof(struct nview_label_data), nview_label_data_free);
        p->text                                 = string_alloc(0);
        p->multi_line                           = 0;
        p->font                                 = NULL;
        p->editable                             = 0;
        p->align                                = NATIVE_UI_LABEL_ALIGN_LEFT;
        p->color                                = (union vec4){1, 1, 1, 1};
        p->dirty                                = 1;
        p->current_width                        = 0;

        return p;
}

void nview_label_data_free(struct nview_label_data *p)
{
        string_free(p->text);
        sfree(p);
}

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
#include <native_ui/align.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/math/math.h>

struct nview_align *nview_align_alloc()
{
        struct nview_align *p     = smalloc(sizeof(struct nview_align), nview_align_free);
        p->type                         = 0;
        p->size_width                   = NATIVE_UI_SIZE_FIXED;
        p->size_height                  = NATIVE_UI_SIZE_FIXED;
        p->fixed_width                  = 0.0f;
        p->fixed_height                 = 0.0f;
        p->margin                       = (union vec4){0, 0, 0, 0};
        p->padding                      = (union vec4){0, 0, 0, 0};
        return p;
}

void nview_align_free(struct nview_align *p)
{
        sfree(p);
}

void nview_align_set_type(struct nview_align *p, u64 type)
{
        p->type = type;
}

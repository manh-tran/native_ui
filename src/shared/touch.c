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
#include <native_ui/touch.h>
#include <cherry/memory.h>
#include <cherry/list.h>

struct nview_touch_data *nview_touch_data_alloc()
{
        struct nview_touch_data *p        = smalloc(sizeof(struct nview_touch_data), nview_touch_data_free);
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->shared_head);
        p->view                                 = NULL;
        return p;
}

void nview_touch_data_free(struct nview_touch_data *p)
{
        list_del(&p->head);
        list_del(&p->shared_head);
        sfree(p);
}

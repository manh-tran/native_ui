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
#include <native_ui/touch_handle.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/stdio.h>

struct native_touch *native_touch_alloc()
{
        struct native_touch *p      = smalloc(sizeof(struct native_touch), native_touch_free);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->head);
        p->parent                               = NULL;
        p->touch_delegate                       = NULL;
        p->custom_data                          = NULL;
        p->custom_data_free                     = NULL;
        p->execute                              = 0;
        return p;
}

void native_touch_free(struct native_touch *p)
{
        if(p->execute >= 1) {
                p->execute = 2;
                return;
        }
        struct list_head *head;
        list_while_not_singular(head, &p->children) {
                struct native_touch *child = (struct native_touch *)
                        ((char *)head - offsetof(struct native_touch, head));
                list_del_init(head);
                native_touch_free(child);
        }
        list_del_init(&p->head);
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
        }
        sfree(p);
}

void native_touch_set_delegate(struct native_touch *p, native_touch_delegate delegate,
        void *custom_data, custom_data_free_delegate free_delegate)
{
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
        }
        p->custom_data          = custom_data;
        p->custom_data_free     = free_delegate;
        p->touch_delegate       = delegate;
}

void native_touch_run(struct native_touch *p, void *sender, u8 type)
{
        struct native_touch *handle = p;
        while(handle) {
                handle->execute = 1;
                if(handle->touch_delegate) {
                        handle->touch_delegate(handle->custom_data, sender, type);
                }
                if(handle->execute > 1) {
                        handle->execute = 0;
                        native_touch_free(handle);
                        return;
                }
                handle->execute = 0;
                handle = handle->parent;
        }
}

void native_touch_link(struct native_touch *p, struct native_touch *c)
{
        list_del(&c->head);
        list_add_tail(&c->head, &p->children);
        c->parent = p;
}

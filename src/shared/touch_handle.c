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

struct ntouch *ntouch_alloc()
{
        struct ntouch *p      = smalloc(sizeof(struct ntouch), ntouch_free);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->head);
        p->parent                               = NULL;
        p->touch_delegate                       = NULL;
        p->custom_data                          = NULL;
        p->custom_data_free                     = NULL;
        p->execute                              = 0;
        return p;
}

void ntouch_free(struct ntouch *p)
{
        if(p->execute >= 1) {
                p->execute = 2;
                return;
        }
        struct list_head *head;
        list_while_not_singular(head, &p->children) {
                struct ntouch *child = (struct ntouch *)
                        ((char *)head - offsetof(struct ntouch, head));
                list_del_init(head);
                ntouch_free(child);
        }
        list_del_init(&p->head);
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
        }
        sfree(p);
}

void ntouch_set_f(struct ntouch *p, ntouchf delegate,
        void *custom_data, custom_data_free_delegate free_delegate)
{
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
        }
        p->custom_data          = custom_data;
        p->custom_data_free     = free_delegate;
        p->touch_delegate       = delegate;
}

void ntouch_run(struct ntouch *p, void *sender, u8 type)
{
        struct ntouch *handle = p;
        while(handle) {
                handle->execute = 1;
                if(handle->touch_delegate) {
                        handle->touch_delegate(handle->custom_data, sender, type);
                }
                if(handle->execute > 1) {
                        handle->execute = 0;
                        ntouch_free(handle);
                        return;
                }
                handle->execute = 0;
                handle = handle->parent;
        }
}

void ntouch_link(struct ntouch *p, struct ntouch *c)
{
        list_del(&c->head);
        list_add_tail(&c->head, &p->children);
        c->parent = p;
}

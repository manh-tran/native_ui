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
#include <native_ui/view_layout_controller.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <native_ui/view.h>
#include <cherry/math/math.h>
#include "controller.h"

struct nlyt_exec *nlyt_exec_alloc()
{
        struct nlyt_exec *p         = smalloc(sizeof(struct nlyt_exec), nlyt_exec_free);
        INIT_LIST_HEAD(&p->head);
        p->type                                         = NATIVE_UI_LAYOUT_ABSOLUTE;
        return p;
}

void nlyt_exec_free(struct nlyt_exec *p)
{
        list_del(&p->head);
        sfree(p);
}

static void __resize_wrap_content_size_vertical(struct nlyt_exec *p)
{
        if(list_singular(&p->head)) return;

        struct nview *view       = (struct nview *)
                ((char *)p->head.next - offsetof(struct nview, layout_controller));

        u8 flag                         = 0;
        struct list_head *child_head;
        union vec2 size                 = view->size;

        switch (view->align->size_width) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.width = 0;
                        list_for_each(child_head, &view->children) {
                                struct nview *sub_child = (struct nview *)
                                        ((char *)child_head - offsetof(struct nview, head));
                                size.width = MAX(size.width,
                                        sub_child->size.width + sub_child->align->margin.left
                                                + sub_child->align->margin.right);
                        }
                        size.width -= view->align->padding.left + view->align->padding.right;
                        view->align->fixed_width = size.width;
                        flag = 1;
                        break;
        }

        if(flag) {
                nview_set_size(view, size);
                nlyt_exec_update_size_vertical(p);
        }
}

static void __resize_wrap_content_vertical(struct nlyt_exec *p)
{
        if(list_singular(&p->head)) return;

        struct nview *view       = (struct nview *)
                ((char *)p->head.next - offsetof(struct nview, layout_controller));

        u8 flag                         = 0;
        struct list_head *child_head;
        union vec2 size                 = view->size;

        switch (view->align->size_height) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.height = 0;
                        list_for_each(child_head, &view->children) {
                                struct nview *sub_child = (struct nview *)
                                        ((char *)child_head - offsetof(struct nview, head));
                                size.height += sub_child->size.height;
                                size.height += sub_child->align->margin.top;
                                size.height += sub_child->align->margin.bottom;
                        }
                        size.height -= view->align->padding.top + view->align->padding.bottom;
                        view->align->fixed_height = size.height;
                        flag = 1;
                        break;
        }
        switch (view->align->size_width) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.width = 0;
                        list_for_each(child_head, &view->children) {
                                struct nview *sub_child = (struct nview *)
                                        ((char *)child_head - offsetof(struct nview, head));
                                size.width = MAX(size.width,
                                        sub_child->size.width + sub_child->align->margin.left
                                                + sub_child->align->margin.right);
                        }
                        size.width -= view->align->padding.left + view->align->padding.right;
                        view->align->fixed_width = size.width;
                        flag = 1;
                        break;
        }

        if(flag) {
                nview_set_size(view, size);
                nlyt_exec_update_vertical(p);
        }
}

static void __resize_wrap_content_horizontal(struct nlyt_exec *p)
{
        if(list_singular(&p->head)) return;

        struct nview *view       = (struct nview *)
                ((char *)p->head.next - offsetof(struct nview, layout_controller));

        u8 flag                         = 0;
        struct list_head *child_head;
        union vec2 size                 = view->size;

        switch (view->align->size_width) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.width = 0;
                        list_for_each(child_head, &view->children) {
                                struct nview *sub_child = (struct nview *)
                                        ((char *)child_head - offsetof(struct nview, head));
                                size.width += sub_child->size.width;
                                size.width += sub_child->align->margin.left;
                                size.width += sub_child->align->margin.right;
                        }
                        size.width -= view->align->padding.left + view->align->padding.right;
                        view->align->fixed_width = size.width;
                        flag = 1;
                        break;
        }
        switch (view->align->size_height) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.height = 0;
                        list_for_each(child_head, &view->children) {
                                struct nview *sub_child = (struct nview *)
                                        ((char *)child_head - offsetof(struct nview, head));
                                size.height = MAX(size.height,
                                        sub_child->size.height + sub_child->align->margin.top
                                                + sub_child->align->margin.bottom);
                        }
                        size.height -= view->align->padding.top + view->align->padding.bottom;
                        view->align->fixed_height = size.height;
                        flag = 1;
                        break;
        }

        if(flag) {
                nview_set_size(view, size);
                nlyt_exec_update_horizontal(p);
        }
}

void nlyt_exec_update(struct nlyt_exec *p)
{
        switch (p->type) {
                case NATIVE_UI_LAYOUT_ABSOLUTE:
                        nlyt_exec_update_absolute(p);
                        break;
                case NATIVE_UI_LAYOUT_RELATIVE:
                        nlyt_exec_update_relative(p);
                        break;
                case NATIVE_UI_LAYOUT_VERTICAL:
                        nlyt_exec_update_vertical(p);
                        __resize_wrap_content_vertical(p);
                        break;
                case NATIVE_UI_LAYOUT_HORIZONTAL:
                        nlyt_exec_update_horizontal(p);
                        __resize_wrap_content_horizontal(p);
                        break;
                case NATIVE_UI_LAYOUT_SIZE_VERTICAL:
                        nlyt_exec_update_size_vertical(p);
                        __resize_wrap_content_size_vertical(p);
                        break;
                case NATIVE_UI_LAYOUT_SIZE_HORIZONTAL:
                        nlyt_exec_update_size_horizontal(p);
                        break;
                default:
                        break;
        }
}

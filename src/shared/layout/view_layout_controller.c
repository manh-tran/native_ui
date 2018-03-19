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
#include <cherry/stdio.h>
#include "controller.h"

struct native_layout *native_layout_alloc()
{
        struct native_layout *p         = smalloc(sizeof(struct native_layout), native_layout_free);
        INIT_LIST_HEAD(&p->head);
        p->type                                         = NATIVE_UI_LAYOUT_ABSOLUTE;
        return p;
}

void native_layout_free(struct native_layout *p)
{
        list_del(&p->head);
        sfree(p);
}

static void __resize_wrap_content_size_horizontal(struct native_layout *p)
{
        if(list_singular(&p->head)) return;

        struct native_view *view       = (struct native_view *)
                ((char *)p->head.next - offsetof(struct native_view, layout_controller));

        u8 flag                         = 0;
        struct list_head *child_head;
        union vec2 size                 = view->size;

        switch (view->align->size_height) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.height = 0;
                        list_for_each(child_head, &view->children) {
                                struct native_view *sub_child = (struct native_view *)
                                        ((char *)child_head - offsetof(struct native_view, head));
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
                native_view_set_size(view, size);
                native_layout_update_size_horizontal(p);
        }
}

static void __resize_wrap_content_size_vertical(struct native_layout *p)
{
        if(list_singular(&p->head)) return;

        struct native_view *view       = (struct native_view *)
                ((char *)p->head.next - offsetof(struct native_view, layout_controller));

        u8 flag                         = 0;
        struct list_head *child_head;
        union vec2 size                 = view->size;

        switch (view->align->size_width) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.width = 0;
                        list_for_each(child_head, &view->children) {
                                struct native_view *sub_child = (struct native_view *)
                                        ((char *)child_head - offsetof(struct native_view, head));
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
                native_view_set_size(view, size);
                native_layout_update_size_vertical(p);
        }
}

static void __resize_wrap_content_vertical(struct native_layout *p)
{
        if(list_singular(&p->head)) return;

        struct native_view *view       = (struct native_view *)
                ((char *)p->head.next - offsetof(struct native_view, layout_controller));

        u8 flag                         = 0;
        struct list_head *child_head;
        union vec2 size                 = view->size;

        switch (view->align->size_height) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.height = 0;
                        list_for_each(child_head, &view->children) {
                                struct native_view *sub_child = (struct native_view *)
                                        ((char *)child_head - offsetof(struct native_view, head));
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
                                struct native_view *sub_child = (struct native_view *)
                                        ((char *)child_head - offsetof(struct native_view, head));
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
                native_view_set_size(view, size);
                native_layout_update_vertical(p);

                size                 = view->size;
                switch (view->align->size_height) {
                        case NATIVE_UI_SIZE_WRAP_CONTENT:
                                size.height = 0;
                                list_for_each(child_head, &view->children) {
                                        struct native_view *sub_child = (struct native_view *)
                                                ((char *)child_head - offsetof(struct native_view, head));
                                        size.height += sub_child->size.height;
                                        size.height += sub_child->align->margin.top;
                                        size.height += sub_child->align->margin.bottom;
                                }
                                size.height -= view->align->padding.top + view->align->padding.bottom;
                                view->align->fixed_height = size.height;
                                break;
                }
                native_view_set_size(view, size);
        }
}

static void __resize_wrap_content_horizontal(struct native_layout *p)
{
        if(list_singular(&p->head)) return;

        struct native_view *view       = (struct native_view *)
                ((char *)p->head.next - offsetof(struct native_view, layout_controller));

        u8 flag                         = 0;
        struct list_head *child_head;
        union vec2 size                 = view->size;

        switch (view->align->size_width) {
                case NATIVE_UI_SIZE_WRAP_CONTENT:
                        size.width = 0;
                        list_for_each(child_head, &view->children) {
                                struct native_view *sub_child = (struct native_view *)
                                        ((char *)child_head - offsetof(struct native_view, head));
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
                                struct native_view *sub_child = (struct native_view *)
                                        ((char *)child_head - offsetof(struct native_view, head));
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
                native_view_set_size(view, size);
                native_layout_update_horizontal(p);
        }
}

void native_layout_update(struct native_layout *p)
{
        switch (p->type) {
                case NATIVE_UI_LAYOUT_ABSOLUTE:
                        native_layout_update_absolute(p);
                        break;
                case NATIVE_UI_LAYOUT_RELATIVE:
                        native_layout_update_relative(p);
                        break;
                case NATIVE_UI_LAYOUT_VERTICAL:
                        native_layout_update_vertical(p);
                        __resize_wrap_content_vertical(p);
                        break;
                case NATIVE_UI_LAYOUT_HORIZONTAL:
                        native_layout_update_horizontal(p);
                        __resize_wrap_content_horizontal(p);
                        break;
                case NATIVE_UI_LAYOUT_SIZE_VERTICAL:
                        native_layout_update_size_vertical(p);
                        __resize_wrap_content_size_vertical(p);
                        break;
                case NATIVE_UI_LAYOUT_SIZE_HORIZONTAL:
                        native_layout_update_size_horizontal(p);
                        __resize_wrap_content_size_horizontal(p);
                        break;
                default:
                        break;
        }
}

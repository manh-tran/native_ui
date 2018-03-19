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
#include "controller.h"
#include <native_ui/view.h>
#include <native_ui/view_layout_controller.h>
#include <cherry/list.h>
#include <cherry/math/math.h>
#include <cherry/stdio.h>

void native_layout_update_size_horizontal(struct native_layout *p)
{
        if(list_singular(&p->head)) return;

        struct native_view *view       = (struct native_view *)
                ((char *)p->head.next - offsetof(struct native_view, layout_controller));

        struct list_head *head;
        union vec2 position             = (union vec2){0, 0};
        float remain_width              = view->size.width;
        float fill_width                = 0;
        int child_count                 = 0;
        int fill_count                  = 0;

        list_for_each_secure(head, &view->children, {
                struct native_view *child = (struct native_view *)
                        ((char *)head - offsetof(struct native_view, head));

                switch (child->align->size_height) {
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                        case NATIVE_UI_SIZE_FILL:
                                child->align->fixed_height              = view->size.height;
                                child->size.height                      = child->align->fixed_height;
                                break;
                }

                switch (child->align->size_width) {
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                child->size.width  = view->size.width;
                                child->size.width -= child->align->padding.left + child->align->padding.right;
                                native_view_set_size(child, child->size);

                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }

                                remain_width -= child->size.width;
                                remain_width -= child->align->margin.left;
                                remain_width -= child->align->margin.right;
                                break;
                        case NATIVE_UI_SIZE_FIXED:
                                child->size.width  = child->align->fixed_width;
                                child->size.width -= child->align->padding.left + child->align->padding.right;
                                native_view_set_size(child, child->size);

                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }

                                remain_width -= child->size.width;
                                remain_width -= child->align->margin.left;
                                remain_width -= child->align->margin.right;
                                break;
                        case NATIVE_UI_SIZE_WEIGHT:
                                remain_width -= child->align->margin.left;
                                remain_width -= child->align->margin.right;
                                break;
                        case NATIVE_UI_SIZE_FILL:
                                fill_count++;
                                remain_width -= child->align->margin.left;
                                remain_width -= child->align->margin.right;
                                break;
                }

                child_count++;
        });

        if(fill_count && remain_width > 0) {
                fill_width = remain_width / fill_count;
        }

        list_for_each_secure(head, &view->children, {
                struct native_view *child = (struct native_view *)
                        ((char *)head - offsetof(struct native_view, head));

                switch (child->align->size_width) {
                        case NATIVE_UI_SIZE_FIXED:
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                position.y = view->size.height/2  + child->size.height * (child->anchor.y - 0.5f);
                                position.x += child->size.width * child->anchor.x;
                                position.x += child->align->margin.left;
                                position.y += child->align->margin.top;
                                position.y -= child->align->margin.bottom;
                                native_view_set_position(child, position);
                                position.x += child->size.width * (1.0f - child->anchor.x);
                                position.x += child->align->margin.right;
                                break;
                        case NATIVE_UI_SIZE_WEIGHT:
                                child->size.width               = remain_width * child->align->weight / view->weights;
                                if(child->type == NATIVE_UI_IMAGE) {
                                        child->size.width -= child->align->padding.left + child->align->padding.right;
                                        switch (child->align->size_height) {
                                                case NATIVE_UI_SIZE_EQUAL:
                                                        child->size.height = child->size.width;
                                                        break;
                                                case NATIVE_UI_SIZE_WRAP_CONTENT:
                                                        if(child->align->fixed_width != 0)
                                                                child->size.height = child->align->fixed_height / child->align->fixed_width * child->size.width;
                                                        else
                                                                child->size.height = 0;
                                                        break;
                                                default:
                                                        child->size.height -= child->align->padding.top + child->align->padding.bottom;
                                                        break;
                                        }
                                } else {
                                        child->size.width -= child->align->padding.left + child->align->padding.right;
                                        child->align->fixed_width      = child->size.width;
                                }

                                native_view_set_size(child, child->size);
                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }
                                position.y = view->size.height / 2  + child->size.height * (child->anchor.y - 0.5f);
                                position.x += child->size.width * child->anchor.x;
                                position.x += child->align->margin.left;
                                position.x += child->align->padding.left;
                                position.y += child->align->margin.top;
                                position.y -= child->align->margin.bottom;
                                native_view_set_position(child, position);
                                position.x += child->size.width * (1.0f - child->anchor.x);
                                position.x += child->align->margin.right;
                                position.x += child->align->padding.right;
                                break;
                        case NATIVE_UI_SIZE_FILL:
                                child->size.width               = fill_width;
                                if(child->type == NATIVE_UI_IMAGE) {
                                        child->size.width -= child->align->padding.left + child->align->padding.right;
                                        switch (child->align->size_height) {
                                                case NATIVE_UI_SIZE_EQUAL:
                                                        child->size.height = child->size.width;
                                                        break;
                                                case NATIVE_UI_SIZE_WRAP_CONTENT:
                                                        if(child->align->fixed_width != 0)
                                                                child->size.height = child->align->fixed_height / child->align->fixed_width * child->size.width;
                                                        else
                                                                child->size.height = 0;
                                                        break;
                                                default:
                                                        child->size.height -= child->align->padding.top + child->align->padding.bottom;
                                                        break;
                                        }
                                } else {
                                        child->size.width -= child->align->padding.left + child->align->padding.right;
                                        child->align->fixed_width      = fill_width;
                                }

                                native_view_set_size(child, child->size);
                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }
                                position.y = view->size.height / 2  + child->size.height * (child->anchor.y - 0.5f);
                                position.x += child->size.width * child->anchor.x;
                                position.x += child->align->margin.left;
                                position.x += child->align->padding.left;
                                position.y += child->align->margin.top;
                                position.y -= child->align->margin.bottom;
                                native_view_set_position(child, position);
                                position.x += child->size.width * (1.0f - child->anchor.x);
                                position.x += child->align->padding.right;
                                position.x += child->align->margin.right;
                                break;
                }
        });

        view->update = 0;
}

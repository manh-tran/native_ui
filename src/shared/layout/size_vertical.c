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

void native_layout_update_size_vertical(struct native_layout *p)
{
        if(list_singular(&p->head)) return;

        struct native_view *view       = (struct native_view *)
                ((char *)p->head.next - offsetof(struct native_view, layout_controller));

        struct list_head *head;
        union vec2 position             = (union vec2){0, 0};
        float remain_height             = view->size.height;
        float fill_height               = 0;
        int child_count                 = 0;
        int fill_count                  = 0;

        list_for_each_secure(head, &view->children, {
                struct native_view *child = (struct native_view *)
                        ((char *)head - offsetof(struct native_view, head));

                switch (child->align->size_width) {
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                        case NATIVE_UI_SIZE_FILL:
                                child->align->fixed_width       = view->size.width;
                                child->size.width               = child->align->fixed_width;
                                break;
                }

                switch (child->align->size_height) {
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                child->size.height = view->size.height;
                                child->size.height -= child->align->padding.top + child->align->padding.bottom;
                                native_view_set_size(child, child->size);
                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }
                                remain_height -= child->size.height;
                                remain_height -= child->align->margin.top;
                                remain_height -= child->align->margin.bottom;
                                break;
                        case NATIVE_UI_SIZE_FIXED:
                                child->size.height = child->align->fixed_height;
                                child->size.height -= child->align->padding.top + child->align->padding.bottom;
                                native_view_set_size(child, child->size);
                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }
                                remain_height -= child->size.height;
                                remain_height -= child->align->margin.top;
                                remain_height -= child->align->margin.bottom;
                                break;
                        case NATIVE_UI_SIZE_WEIGHT:
                                remain_height -= child->align->margin.top;
                                remain_height -= child->align->margin.bottom;
                                break;
                        case NATIVE_UI_SIZE_FILL:
                                fill_count++;
                                remain_height -= child->align->margin.top;
                                remain_height -= child->align->margin.bottom;
                                break;
                }

                child_count++;
        });

        if(fill_count && remain_height > 0) {
                fill_height = remain_height / fill_count;
        }

        list_for_each_secure(head, &view->children, {
                struct native_view *child = (struct native_view *)
                        ((char *)head - offsetof(struct native_view, head));

                switch (child->align->size_height) {
                        case NATIVE_UI_SIZE_FIXED:
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                position.x = view->size.width/2  + child->size.width * (child->anchor.x - 0.5f);
                                position.y += child->size.height * child->anchor.y;
                                position.x += child->align->margin.left;
                                position.x -= child->align->margin.right;
                                position.y += child->align->margin.top;
                                native_view_set_position(child, position);
                                position.y += child->size.height * (1.0f - child->anchor.y);
                                position.y += child->align->margin.bottom;
                                break;
                        case NATIVE_UI_SIZE_WEIGHT:
                                child->size.height              = remain_height * child->align->weight / view->weights;
                                if(child->type == NATIVE_UI_IMAGE) {
                                        child->size.height -= child->align->padding.top + child->align->padding.bottom;
                                        switch (child->align->size_width) {
                                                case NATIVE_UI_SIZE_EQUAL:
                                                        child->size.width = child->size.height;
                                                        break;
                                                case NATIVE_UI_SIZE_WRAP_CONTENT:
                                                        if(child->align->fixed_height != 0)
                                                                child->size.width = child->align->fixed_width / child->align->fixed_height * child->size.height;
                                                        else
                                                                child->size.width = 0;
                                                        break;
                                                default:
                                                        child->size.width -= child->align->padding.left + child->align->padding.right;
                                                        break;
                                        }
                                } else {
                                        child->align->fixed_height      = fill_height;
                                }
                                native_view_set_size(child, child->size);
                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }
                                position.x = view->size.width / 2  + child->size.width * (child->anchor.x - 0.5f);
                                position.y += child->size.height * child->anchor.y;
                                position.x += child->align->margin.left;
                                position.x -= child->align->margin.right;
                                position.y += child->align->margin.top;
                                position.y += child->align->padding.top;
                                native_view_set_position(child, position);
                                position.y += child->size.height * (1.0f - child->anchor.y);
                                position.y += child->align->margin.bottom;
                                position.y += child->align->padding.bottom;
                                break;
                        case NATIVE_UI_SIZE_FILL:
                                child->size.height              = fill_height;
                                if(child->type == NATIVE_UI_IMAGE) {
                                        child->size.height -= child->align->padding.top + child->align->padding.bottom;
                                        switch (child->align->size_width) {
                                                case NATIVE_UI_SIZE_EQUAL:
                                                        child->size.width = child->size.height;
                                                        break;
                                                case NATIVE_UI_SIZE_WRAP_CONTENT:
                                                        if(child->align->fixed_height != 0)
                                                                child->size.width = child->align->fixed_width / child->align->fixed_height * child->size.height;
                                                        else
                                                                child->size.width = 0;
                                                        break;
                                                default:
                                                        child->size.width -= child->align->padding.left + child->align->padding.right;
                                                        break;
                                        }
                                } else {
                                        child->align->fixed_height      = fill_height;
                                }
                                native_view_set_size(child, child->size);
                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        native_view_update_layout(child);
                                }
                                position.x = view->size.width / 2  + child->size.width * (child->anchor.x - 0.5f);
                                position.y += child->size.height  * child->anchor.y;
                                position.x += child->align->margin.left;
                                position.x -= child->align->margin.right;
                                position.y += child->align->margin.top;
                                position.y += child->align->padding.top;
                                native_view_set_position(child, position);
                                position.y += child->size.height  * (1.0f - child->anchor.y);
                                position.y += child->align->margin.bottom;
                                position.y += child->align->padding.bottom;
                                break;
                }
        });

        view->update = 0;
}

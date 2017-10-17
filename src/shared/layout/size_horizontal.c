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

void nlyt_exec_update_size_horizontal(struct nlyt_exec *p)
{
        if(list_singular(&p->head)) return;

        struct nview *view       = (struct nview *)
                ((char *)p->head.next - offsetof(struct nview, layout_controller));

        struct list_head *head;
        union vec2 position             = (union vec2){0, 0};
        float remain_width              = view->size.width;
        float fill_width                = 0;
        int child_count                 = 0;
        int fill_count                  = 0;

        list_for_each_secure(head, &view->children, {
                struct nview *child = (struct nview *)
                        ((char *)head - offsetof(struct nview, head));

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
                                nview_set_size(child, child->size);

                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        nview_update_layout(child);
                                }

                                remain_width -= child->size.width;
                                remain_width -= child->align->margin.left;
                                remain_width -= child->align->margin.right;
                                break;
                        case NATIVE_UI_SIZE_FIXED:
                                child->size.width  = child->align->fixed_width;
                                child->size.width -= child->align->padding.left + child->align->padding.right;
                                nview_set_size(child, child->size);

                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        nview_update_layout(child);
                                }

                                remain_width -= child->size.width;
                                remain_width -= child->align->margin.left;
                                remain_width -= child->align->margin.right;
                                break;
                        case NATIVE_UI_SIZE_FILL:
                                fill_count++;
                                remain_width -= child->align->margin.left;
                                remain_width -= child->align->margin.right;
                                remain_width += child->align->padding.left;
                                remain_width += child->align->padding.right;
                                break;
                }

                child_count++;
        });

        if(fill_count && remain_width > 0) {
                fill_width = remain_width / fill_count;
        }

        list_for_each_secure(head, &view->children, {
                struct nview *child = (struct nview *)
                        ((char *)head - offsetof(struct nview, head));

                switch (child->align->size_width) {
                        case NATIVE_UI_SIZE_FIXED:
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                position.y = view->size.height/2  + child->size.height * (child->anchor.y - 0.5f);
                                position.x += child->size.width * child->anchor.x;
                                position.x += child->align->margin.left;
                                position.y += child->align->margin.top;
                                position.y -= child->align->margin.bottom;
                                nview_set_position(child, position);
                                position.x += child->size.width * (1.0f - child->anchor.x);
                                position.x += child->align->margin.right;
                                break;
                        case NATIVE_UI_SIZE_FILL:
                                child->align->fixed_width       = fill_width;
                                child->size.width               = fill_width;
                                nview_set_size(child, child->size);
                                if(child->update & NATIVE_UI_UPDATE_CHILDREN)
                                {
                                        nview_update_layout(child);
                                }
                                position.y = view->size.height / 2  + child->size.height * (child->anchor.y - 0.5f);
                                position.x += fill_width * child->anchor.x;
                                position.x += child->align->margin.left;
                                position.y += child->align->margin.top;
                                position.y -= child->align->margin.bottom;
                                nview_set_position(child, position);
                                position.x += fill_width * (1.0f - child->anchor.x);
                                position.x += child->align->margin.right;
                                break;
                }
        });

        view->update = 0;
}

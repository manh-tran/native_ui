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

void nlyt_exec_update_horizontal(struct nlyt_exec *p)
{
        if(list_singular(&p->head)) return;

        struct nview *view       = (struct nview *)
                ((char *)p->head.next - offsetof(struct nview, layout_controller));

        struct list_head *head;
        union vec2 position = (union vec2){0, 0};
        view->bound                        = (union vec2){0, 0};

        list_for_each_secure(head, &view->children, {
                struct nview *child = (struct nview *)
                        ((char *)head - offsetof(struct nview, head));

                /*
                 * reset child size
                 */
                union vec2 size;
                size.width      = child->align->fixed_width;
                size.height     = child->align->fixed_height;

                /*
                 * check match_parent
                 */
                switch (child->align->size_width) {
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                size.width     = view->size.width;
                                break;
                }
                switch (child->align->size_height) {
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                size.height     = view->size.height;
                                break;
                }
                size.width -= child->align->padding.left + child->align->padding.right;
                size.height -= child->align->padding.top + child->align->padding.bottom;
                /*
                 * check wrap
                 */
                if(child->type == NATIVE_UI_IMAGE) {
                        switch (child->align->size_width) {
                                case NATIVE_UI_SIZE_WRAP_CONTENT:
                                        if(child->align->fixed_height != 0)
                                                size.width = child->align->fixed_width / child->align->fixed_height * size.height;
                                        else
                                                size.width = 0;
                                        break;
                        }
                        switch (child->align->size_height) {
                                case NATIVE_UI_SIZE_WRAP_CONTENT:
                                        if(child->align->fixed_width != 0)
                                                size.height = child->align->fixed_height / child->align->fixed_width * size.width;
                                        else
                                                size.height = 0;
                                        break;
                        }

                }

                nview_set_size(child, size);

                if(child->update & NATIVE_UI_UPDATE_CHILDREN) 
                {
                        nview_update_layout(child);
                        size = child->size;
                }

                /*
                 * reset child position
                 */
                position.x += size.width * child->anchor.x;

                if(child->align->type & NATIVE_UI_ALIGN_CENTER) {
                        position.y = view->size.height/2 + size.height * (child->anchor.y - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_TOP) {
                        position.y = child->size.height * child->anchor.y;
                }
                if(child->align->type & NATIVE_UI_ALIGN_BOTTOM) {
                        position.y = view->size.height - size.height/2  + size.height * (child->anchor.y - 0.5f);
                }
                position.x += child->align->margin.left;
                position.y += child->align->margin.top;
                nview_set_position(child, vec2_add(view->offset_child, position));

                position.x += child->align->margin.right;
                position.x += size.width * (1.0f - child->anchor.x);
                view->bound.width = position.x;
        });

        view->update = 0;
}

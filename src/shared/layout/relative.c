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

void nlyt_exec_update_relative(struct nlyt_exec *p)
{
        if(list_singular(&p->head)) return;

        struct nview *view       = (struct nview *)
                ((char *)p->head.next - offsetof(struct nview, layout_controller));

        struct list_head *head;
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
                 * check match parent
                 */
                switch (child->align->size_width) {
                        case NATIVE_UI_SIZE_MATCH_PARENT:
                                size.width      = view->size.width;
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
                nview_update_label(child);
                size = child->size;
                /*
                 * resert size position
                 */
                union vec2 position;
                position.x = position.y = 0;
                if(child->align->type & NATIVE_UI_ALIGN_CENTER) {
                        position.x = view->size.width/2  + size.width * (child->anchor.x - 0.5f);
                        position.y = view->size.height/2  + size.height * (child->anchor.y - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_TOP) {
                        position.y = child->size.height/2  + size.height * (child->anchor.y - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_OUT_TOP) {
                        position.y = -child->size.height/2  + size.height * (child->anchor.y - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_LEFT) {
                        position.x = child->size.width/2  + size.width * (child->anchor.x - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_OUT_LEFT) {
                        position.x = -child->size.width/2  + size.width * (child->anchor.x - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_BOTTOM) {
                        position.y = view->size.height - child->size.height/2 + size.height * (child->anchor.y - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_OUT_BOTTOM) {
                        position.y = view->size.height + child->size.height/2 + size.height * (child->anchor.y - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_RIGHT) {
                        position.x = view->size.width - child->size.width/2  + size.width * (child->anchor.x - 0.5f);
                }
                if(child->align->type & NATIVE_UI_ALIGN_OUT_RIGHT) {
                        position.x = view->size.width + child->size.width/2  + size.width * (child->anchor.x - 0.5f);
                }
                position.x += child->align->margin.left;
                position.x -= child->align->margin.right;
                position.y += child->align->margin.top;
                position.y -= child->align->margin.bottom;
                nview_set_position(child, position);
        });

        view->update = 0;
}

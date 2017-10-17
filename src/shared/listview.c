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
#include <native_ui/view.h>
#include <native_ui/view_layout_controller.h>
#include <native_ui/align.h>
#include <native_ui/parser.h>
#include <native_ui/manager.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>

void __nview_list_view_data_free(struct nview_list_view_data *p)
{
        sfree(p);
}

static struct nview_list_view_data *__nview_list_view_data_alloc()
{
        struct nview_list_view_data *p    = smalloc(sizeof(struct nview_list_view_data), __nview_list_view_data_free);
        p->bouncing                             = 0;
        p->hold                                 = 0;
        p->scrolling                            = 0;
        p->velocity                             = (union vec2){0, 0};
        p->acceleration                         = (union vec2){0, 0};
        p->stop                                 = 0;
        return p;
}

void nview_on_change_listview(struct nview *p)
{
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
        }

        p->custom_data          = __nview_list_view_data_alloc();
        p->custom_data_free     = __nview_list_view_data_free;
}

static void __list_view_update_vertical(struct nview *p, float delta)
{
        struct nview_list_view_data *data = (struct nview_list_view_data *)p->custom_data;

        float target_bottom = p->bound.height - p->size.height;
        if(target_bottom < 0) target_bottom = 0;

        if( ! data->stop) {
                p->offset_child.x       = p->offset_child.x + data->velocity.x * delta + data->acceleration.x * delta * delta * 0.5f;
                p->offset_child.y       = p->offset_child.y + data->velocity.y * delta + data->acceleration.y * delta * delta * 0.5f;

                data->velocity.x        = data->velocity.x + data->acceleration.x * delta;
                data->velocity.y        = data->velocity.y + data->acceleration.y * delta;

                union vec2 norm_veloc   = vec2_normalize(data->velocity);
                union vec2 norm_accel   = vec2_normalize(data->acceleration);

                if(data->velocity.y > 0) {
                        if((p->offset_child.y < -target_bottom || p->offset_child.y > 0) && fabsf(data->velocity.y) <= 30.0f) {
                                data->stop = 1;
                        }
                        if(p->offset_child.y >= 40 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                        if(p->offset_child.y >= 140 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                } else if(data->velocity.y < 0) {
                        if((p->offset_child.y < -target_bottom || p->offset_child.y > 0) && fabsf(data->velocity.y) <= 30.0f) {
                                data->stop = 1;
                        }
                        if(p->offset_child.y < -target_bottom - 40 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                        if(p->offset_child.y < -target_bottom - 140 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                }

                if(data->hold) {
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);

                } else {
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);
                }

                if(vec2_dot(norm_veloc, norm_accel) >= 0) {
                        data->stop = 1;
                }
        } else if(!data->hold){
                if(p->offset_child.y > 0) {
                        if(!data->bouncing) {
                                data->bouncing = 1;
                                data->velocity.y = -p->offset_child.y * 4;
                                data->acceleration.y = p->offset_child.y * 6;
                                data->velocity.y = data->velocity.y * 1;
                                data->acceleration.y = data->acceleration.y * 0.05;
                        } else if(data->bouncing) {
                                if(p->offset_child.y > 0) {
                                        p->offset_child.x       = p->offset_child.x + data->velocity.x * delta + data->acceleration.x * delta * delta * 0.5f;
                                        p->offset_child.y       = p->offset_child.y + data->velocity.y * delta + data->acceleration.y * delta * delta * 0.5f;

                                        data->velocity.x        = data->velocity.x + data->acceleration.x * delta;
                                        data->velocity.y        = data->velocity.y + data->acceleration.y * delta;

                                        if(p->offset_child.y <= 0) {
                                                p->offset_child.y = 0;
                                                nview_unschedule(p, qlkey("list_view_update"));
                                                data->scrolling = 0;
                                        }
                                        data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                }
                        }
                } else if(p->offset_child.y < 0) {
                        if(!data->bouncing) {
                                data->bouncing = 1;
                                data->velocity.y = (-p->offset_child.y - target_bottom) * 4;
                                data->acceleration.y = -(-p->offset_child.y - target_bottom) * 6;
                                data->velocity.y = data->velocity.y * 1;
                                data->acceleration.y = data->acceleration.y * 0.05;

                                if(p->offset_child.y > -target_bottom) {
                                        nview_unschedule(p, qlkey("list_view_update"));
                                        data->scrolling = 0;
                                        goto update_layout;
                                }

                        } else if(data->bouncing) {
                                if(p->offset_child.y < -target_bottom) {

                                        p->offset_child.x       = p->offset_child.x + data->velocity.x * delta + data->acceleration.x * delta * delta * 0.5f;
                                        p->offset_child.y       = p->offset_child.y + data->velocity.y * delta + data->acceleration.y * delta * delta * 0.5f;

                                        data->velocity.x        = data->velocity.x + data->acceleration.x * delta;
                                        data->velocity.y        = data->velocity.y + data->acceleration.y * delta;

                                        if(p->offset_child.y >= -target_bottom) {
                                                p->offset_child.y = -target_bottom;
                                                nview_unschedule(p, qlkey("list_view_update"));
                                                data->scrolling = 0;
                                        }
                                        data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                }
                        }
                }

        }
update_layout:;
        nview_update_layout(p);
}

static void __list_view_update_horizontal(struct nview *p, float delta)
{
        struct nview_list_view_data *data = (struct nview_list_view_data *)p->custom_data;

        float target_bottom = p->bound.width - p->size.width;
        if(target_bottom < 0) target_bottom = 0;

        if( ! data->stop) {
                p->offset_child.x       = p->offset_child.x + data->velocity.x * delta + data->acceleration.x * delta * delta * 0.5f;
                p->offset_child.y       = p->offset_child.y + data->velocity.y * delta + data->acceleration.y * delta * delta * 0.5f;

                data->velocity.x        = data->velocity.x + data->acceleration.x * delta;
                data->velocity.y        = data->velocity.y + data->acceleration.y * delta;

                union vec2 norm_veloc   = vec2_normalize(data->velocity);
                union vec2 norm_accel   = vec2_normalize(data->acceleration);

                if(data->velocity.x > 0) {
                        if((p->offset_child.x < -target_bottom || p->offset_child.x > 0) && fabsf(data->velocity.x) <= 30.0f) {
                                data->stop = 1;
                        }
                        if(p->offset_child.x >= 40 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                        if(p->offset_child.x >= 140 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                } else if(data->velocity.x < 0) {
                        if((p->offset_child.x < -target_bottom || p->offset_child.x > 0) && fabsf(data->velocity.x) <= 30.0f) {
                                data->stop = 1;
                        }
                        if(p->offset_child.x < -target_bottom - 40 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                        if(p->offset_child.x < -target_bottom - 140 && !data->hold) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        }
                }

                if(data->hold) {
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.9f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);

                } else {
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);
                        data->velocity          = vec2_mul_scalar(data->velocity, 0.992f);
                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.3f);

                }

                if(vec2_dot(norm_veloc, norm_accel) >= 0) {
                        data->stop = 1;
                }
        } else if(!data->hold){
                if(p->offset_child.x > 0) {
                        if(!data->bouncing) {
                                data->bouncing = 1;
                                data->velocity.x = -p->offset_child.x * 4;
                                data->acceleration.x = p->offset_child.x * 6;
                                data->velocity.x = data->velocity.x * 1;
                                data->acceleration.x = data->acceleration.x * 0.05;
                        } else if(data->bouncing) {
                                if(p->offset_child.x > 0) {
                                        p->offset_child.x       = p->offset_child.x + data->velocity.x * delta + data->acceleration.x * delta * delta * 0.5f;
                                        p->offset_child.y       = p->offset_child.y + data->velocity.y * delta + data->acceleration.y * delta * delta * 0.5f;

                                        data->velocity.x        = data->velocity.x + data->acceleration.x * delta;
                                        data->velocity.y        = data->velocity.y + data->acceleration.y * delta;

                                        if(p->offset_child.x <= 0) {
                                                p->offset_child.x = 0;
                                                nview_unschedule(p, qlkey("list_view_update"));
                                                data->scrolling = 0;
                                        }
                                        data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                }
                        }
                } else if(p->offset_child.x < 0) {
                        if(!data->bouncing) {
                                data->bouncing = 1;
                                data->velocity.x = (-p->offset_child.x - target_bottom) * 4;
                                data->acceleration.x = -(-p->offset_child.x - target_bottom) * 6;
                                data->velocity.x = data->velocity.x * 1;
                                data->acceleration.x = data->acceleration.x * 0.05;

                                if(p->offset_child.x > -target_bottom) {
                                        nview_unschedule(p, qlkey("list_view_update"));
                                        data->scrolling = 0;
                                        goto update_layout;
                                }

                        } else if(data->bouncing) {
                                if(p->offset_child.x < -target_bottom) {

                                        p->offset_child.x       = p->offset_child.x + data->velocity.x * delta + data->acceleration.x * delta * delta * 0.5f;
                                        p->offset_child.y       = p->offset_child.y + data->velocity.y * delta + data->acceleration.y * delta * delta * 0.5f;

                                        data->velocity.x        = data->velocity.x + data->acceleration.x * delta;
                                        data->velocity.y        = data->velocity.y + data->acceleration.y * delta;

                                        if(p->offset_child.x >= -target_bottom) {
                                                p->offset_child.x = -target_bottom;
                                                nview_unschedule(p, qlkey("list_view_update"));
                                                data->scrolling = 0;
                                        }
                                        data->velocity          = vec2_mul_scalar(data->velocity, 0.95f);
                                        data->acceleration      = vec2_mul_scalar(data->acceleration, 0.6f);
                                }
                        }
                }

        }
update_layout:;
        nview_update_layout(p);
}

static void __list_view_update(struct nview *p, float delta)
{
        int layout_type                                                 = -1;
        if(!list_singular(&p->layout_controller)) {
                struct nlyt_exec *controller        = (struct nlyt_exec *)
                        ((char *)p->layout_controller.next - offsetof(struct nlyt_exec, head));
                layout_type                                             = controller->type;
        }
        switch (layout_type) {
                case NATIVE_UI_LAYOUT_HORIZONTAL:
                        __list_view_update_horizontal(p, delta);
                        break;
                case NATIVE_UI_LAYOUT_VERTICAL:
                        __list_view_update_vertical(p, delta);
                        break;
                default:
                        break;
        }
}

void nview_list_view_touch_began(struct nview *p, union vec2 liv)
{
        // nview_schedule(p, qlkey("list_view_update"), -1, __list_view_update);
        struct nview_list_view_data *data = (struct nview_list_view_data *)p->custom_data;
        data->hold                              = 1;
        data->bouncing                          = 0;
        data->stop                              = 1;
        data->last_moved_point                  = liv;
}

void nview_list_view_touch_moved(struct nview *p, union vec2 liv)
{
        int layout_type                                                 = -1;
        if(!list_singular(&p->layout_controller)) {
                struct nlyt_exec *controller        = (struct nlyt_exec *)
                        ((char *)p->layout_controller.next - offsetof(struct nlyt_exec, head));
                layout_type                                             = controller->type;
        }
        struct nview_list_view_data *data = (struct nview_list_view_data *)p->custom_data;
        if(!data->scrolling) {
                switch (layout_type) {
                        case NATIVE_UI_LAYOUT_HORIZONTAL:
                                if(fabsf(liv.x - data->last_moved_point.x) >= 10) {
                                        nview_schedule(p, qlkey("list_view_update"), -1, __list_view_update);
                                        data->scrolling = 1;
                                }
                                break;
                        case NATIVE_UI_LAYOUT_VERTICAL:
                                if(fabsf(liv.y - data->last_moved_point.y) >= 10) {
                                        nview_schedule(p, qlkey("list_view_update"), -1, __list_view_update);
                                        data->scrolling = 1;
                                }
                                break;
                        default:
                                break;
                }
        }
        if(!data->scrolling) return;

        float target_bottom = 0;

        /*
         * 0.358486 = 0.95 ^ 20
         * 0.000037 = 0.6 ^ 20
         */
        switch (layout_type) {
                case NATIVE_UI_LAYOUT_HORIZONTAL:
                #if OS == IOS
                        if(fabsf(liv.x - data->last_moved_point.x) < 1) return;
                #else
                        if(fabsf(liv.x - data->last_moved_point.x) < 0.5) return;
                #endif

                        data->velocity.x        = (liv.x - data->last_moved_point.x) * 60;
                        data->acceleration.x    = -(liv.x - data->last_moved_point.x) * 100;
                        data->velocity.y        = 0;
                        data->acceleration.y    = 0;
                        target_bottom           = p->bound.width - p->size.width;

                        if(p->offset_child.x > 0) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.x > 50) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.x > 100) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.x > 150) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }

                        if(p->offset_child.x < -target_bottom) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.x < -target_bottom - 100) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.x < -target_bottom - 200) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.x < -target_bottom - 300) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        break;
                case NATIVE_UI_LAYOUT_VERTICAL:
                #if OS == IOS
                        if(fabsf(liv.y - data->last_moved_point.y) < 1) return;
                #else
                        if(fabsf(liv.y - data->last_moved_point.y) < 0.5) return;
                #endif

                        data->velocity.y        = (liv.y - data->last_moved_point.y) * 60;
                        data->acceleration.y    = -(liv.y - data->last_moved_point.y) * 100;
                        data->velocity.x        = 0;
                        data->acceleration.x    = 0;
                        target_bottom           = p->bound.height - p->size.height;

                        if(p->offset_child.y > 0) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.y > 50) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.y > 100) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.y > 150) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }

                        if(p->offset_child.y < -target_bottom) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.y < -target_bottom - 100) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.y < -target_bottom - 200) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        if(p->offset_child.y < -target_bottom - 300) {
                                data->velocity          = vec2_mul_scalar(data->velocity, 0.358486);
                                data->acceleration      = vec2_mul_scalar(data->acceleration, 0.000037);
                        }
                        break;
                default:
                        break;
        }

        data->hold                              = 1;
        data->bouncing                          = 0;
        data->stop                              = 0;
        data->last_moved_point                  = liv;
}

void nview_list_view_touch_ended(struct nview *p, union vec2 liv)
{
        struct nview_list_view_data *data = (struct nview_list_view_data *)p->custom_data;
        data->hold                              = 0;
}

void nview_list_view_touch_cancelled(struct nview *p, union vec2 liv)
{
        struct nview_list_view_data *data = (struct nview_list_view_data *)p->custom_data;
        data->hold                              = 0;
}

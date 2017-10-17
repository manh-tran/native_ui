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
#include <native_ui/view_controller.h>
#include <native_ui/align.h>
#include <native_ui/parser.h>
#include <native_ui/manager.h>
#include <native_ui/touch_handle.h>
#include <native_ui/action.h>
#include <native_ui/touch.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/math/math.h>

struct intercept_head {
        struct list_head        head;
        struct list_head        sib_head;

        struct nview      *view;
};

static void __intercept_head_free(struct intercept_head *p)
{
        list_del(&p->head);
        list_del(&p->sib_head);
        sfree(p);
}

static struct intercept_head *__intercept_head_alloc()
{
        struct intercept_head *p        = smalloc(sizeof(struct intercept_head), __intercept_head_free);
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->sib_head);
        p->view = NULL;
        return p;
}

void nview_init(struct nview *p)
{
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        p->parent                       = NULL;
        p->name_to_child                = NULL;

        INIT_LIST_HEAD(&p->layout_controller);
        p->align                        = nview_align_alloc();
        p->update                       = NATIVE_UI_UPDATE_SELF | NATIVE_UI_UPDATE_CHILDREN;

        INIT_LIST_HEAD(&p->view_controller);

        p->user_interaction_enabled     = 0;
        p->touch_began_delegate         = NULL;
        p->touch_ended_delegate         = NULL;
        p->touch_moved_delegate         = NULL;
        p->touch_cancelled_delegate     = NULL;
        p->text_done_delegate           = NULL;

        p->position                     = (union vec2){0, 0};
        p->size                         = (union vec2){0, 0};
        p->scale                        = (union vec2){1, 1};
        p->color                        = NULL;
        p->border_color                 = NULL;
        p->rotation                     = (union vec3){0, 0, 0};
        p->anchor                       = (union vec2){0.5, 0.5};
        p->name                         = string_alloc(0);
        p->offset_child                 = (union vec2){0, 0};
        p->bound                        = (union vec2){0, 0};
        p->clip                         = 0;
        p->clip_rounds                  = (union vec4){0, 0, 0, 0};
        p->alpha                        = 1.0f;
        p->visible                      = 1;

        p->touch_began_point            = (union vec2){0, 0};
        p->touch_moved_point            = (union vec2){0, 0};
        p->touch_ended_point            = (union vec2){0, 0};
        p->touch_offset                 = (union vec2){0, 0};

        INIT_LIST_HEAD(&p->parser);
        INIT_LIST_HEAD(&p->hash_head);

        p->type                         = -1;

        p->custom_data                  = NULL;
        p->custom_data_free             = NULL;

        p->user_data                    = NULL;
        p->user_data_free               = NULL;

        p->tasks                        = map_alloc(sizeof(struct list_head *));

        p->parser_touch_handle          = NULL;
        INIT_LIST_HEAD(&p->intercept_horizontal);
        INIT_LIST_HEAD(&p->intercept_horizontal_head);
        INIT_LIST_HEAD(&p->intercept_vertical);
        INIT_LIST_HEAD(&p->intercept_vertical_head);
        p->intercept_vertical_target    = NULL;
        p->intercept_horizontal_target  = NULL;

        naction_key_init(&p->action_key);

        INIT_LIST_HEAD(&p->touch_list);
}

static void __free_task(struct list_head *head)
{
        if(!list_singular(head)) {
                struct ntask *task = (struct ntask *)
                        ((char *)head->next - offsetof(struct ntask, user_head));
                ntask_free(task);
        }
        sfree(head);
}

void nview_free_common(struct nview *p)
{
        struct list_head *head;
        /*
         * remove touch
         */
        list_for_each_secure_mutex_lock(head, &p->touch_list, &__shared_nview_touch_lock, {
                struct nview_touch_data *data = (struct nview_touch_data *)
                        ((char *)head - offsetof(struct nview_touch_data, head));
                nview_touch_data_free(data);
        });
        /*
         * remove sub views
         */
        list_while_not_singular(head, &p->children) {
                struct nview *child       = (struct nview *)
                        ((char *)head - offsetof(struct nview, head));
                nview_free(child);
        }
        /*
         * detach view from parent
         */
        if(p->name->len) {
                if(p->parent && p->parent->name_to_child) {
                        map_remove_key(p->parent->name_to_child, qskey(p->name));
                }
        }
        list_del(&p->head);
        p->parent = NULL;
        string_free(p->name);
        if(p->name_to_child) {
                map_free(p->name_to_child);
        }
        /*
         * clear parser
         */
        list_del_init(&p->hash_head);
        if(!list_singular(&p->parser)) {
                struct nparser *parser = (struct nparser *)
                        ((char *)p->parser.next - offsetof(struct nparser, view));
                list_del_init(&p->parser);
                nparser_free(parser);
        }
        list_del_init(&p->parser);
        /*
         * detach layout controller
         */
        if(!list_singular(&p->layout_controller)) {
                struct nlyt_exec *controller = (struct nlyt_exec *)
                        ((char *)p->layout_controller.next - offsetof(struct nlyt_exec, head));
                nlyt_exec_free(controller);
        }
        nview_align_free(p->align);
        /*
         * detach user controller
         */
        if(!list_singular(&p->view_controller)) {
                struct nexec *controller = (struct nexec *)
                        ((char *)p->view_controller.next - offsetof(struct nexec, view));
                list_del_init(&p->view_controller);
                nexec_free(controller);
        }
        /*
         * clear custom data
         */
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
                p->custom_data             = NULL;
                p->custom_data_free        = NULL;
        }
        if(p->user_data && p->user_data_free) {
                p->user_data_free(p->user_data);
                p->user_data            = NULL;
                p->user_data_free       = NULL;
        }
        /*
         * free update task
         */
        map_deep_free(p->tasks, struct list_head *, __free_task);
        if(p->color) {
                sfree(p->color);
                p->color        = NULL;
        }
        if(p->border_color) {
                sfree(p->border_color);
                p->border_color = NULL;
        }
        /*
         * delete intercept touch
         */
        list_while_not_singular(head, &p->intercept_horizontal_head) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)head - offsetof(struct intercept_head, sib_head));
                __intercept_head_free(ih);
        }
        list_while_not_singular(head, &p->intercept_vertical_head) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)head - offsetof(struct intercept_head, sib_head));
                __intercept_head_free(ih);
        }
        p->intercept_horizontal_target  = NULL;
        p->intercept_vertical_target    = NULL;

        naction_key_clear(&p->action_key);
}

void nview_remove_all_children(struct nview *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->children) {
                struct nview *child       = (struct nview *)
                        ((char *)head - offsetof(struct nview, head));
                nview_free(child);
        }
}

void nview_set_intercept_horizontal(struct nview *from, struct nview *to)
{
        if(!list_singular(&from->intercept_horizontal)) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)from->intercept_horizontal.next - offsetof(struct intercept_head, head));
                __intercept_head_free(ih);
        }

        struct intercept_head *head = __intercept_head_alloc();
        list_add_tail(&head->head, &from->intercept_horizontal);
        list_add_tail(&head->sib_head, &to->intercept_horizontal_head);
        head->view = to;
}

void nview_set_intercept_vertical(struct nview *from, struct nview *to)
{
        if(!list_singular(&from->intercept_vertical)) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)from->intercept_vertical.next - offsetof(struct intercept_head, head));
                __intercept_head_free(ih);
        }

        struct intercept_head *head = __intercept_head_alloc();
        list_add_tail(&head->head, &from->intercept_vertical);
        list_add_tail(&head->sib_head, &to->intercept_vertical_head);
        head->view = to;
}

struct nparser *nview_get_parser(struct nview *p)
{
        if(!list_singular(&p->parser)) {
                struct nparser *parser = (struct nparser *)
                        ((char *)p->parser.next - offsetof(struct nparser, view));
                return parser;
        }
        return NULL;
}

void nview_schedule(struct nview *p, char *key, size_t len, int count, ntaskf delegate)
{
        if(map_has_key(p->tasks, key, len)) {
                struct list_head *head = map_get(p->tasks, struct list_head *, key, len);
                __free_task(head);
                map_remove_key(p->tasks, key, len);
        }

        struct list_head *head   = smalloc(sizeof(struct list_head), NULL);
        INIT_LIST_HEAD(head);

        struct ntask *task = ntask_alloc();
        list_add_tail(&task->user_head, head);
        task->count     = count;
        task->data      = p;
        task->delegate  = delegate;

        map_set(p->tasks, key, len, &head);
}

void nview_unschedule(struct nview *p, char *key, size_t len)
{
        if(map_has_key(p->tasks, key, len)) {
                struct list_head *head = map_get(p->tasks, struct list_head *, key, len);
                __free_task(head);
                map_remove_key(p->tasks, key, len);
        }
}

void nview_update_layout(struct nview *p)
{
        if(!list_singular(&p->layout_controller)) {
                struct nlyt_exec *controller = (struct nlyt_exec *)
                        ((char *)p->layout_controller.next - offsetof(struct nlyt_exec, head));
                nlyt_exec_update(controller);
        } else {
                union vec2 size = p->size;
                if(p->align->size_width == NATIVE_UI_SIZE_FIXED)
                        size.width = p->align->fixed_width;
                if(p->align->size_height == NATIVE_UI_SIZE_FIXED)
                        size.height = p->align->fixed_height;

                nview_set_size(p, size);
                nview_update_label(p);
        }
}

void nview_request_layout(struct nview *p)
{
        if(p->parent) {
                struct nview *parent = p->parent;
                struct nview *super_parent = parent->parent;
                if(!list_singular(&parent->layout_controller)) {
                        goto check_super_parent;
                } else {
                        goto update;
                }

        check_super_parent:;
                if(super_parent) {
                        if(!list_singular(&super_parent->layout_controller)) {
                                struct nlyt_exec *controller = (struct nlyt_exec *)
                                        ((char *)super_parent->layout_controller.next - offsetof(struct nlyt_exec, head));
                                switch (p->type) {
                                        case NATIVE_UI_LAYOUT_VERTICAL:
                                        case NATIVE_UI_LAYOUT_HORIZONTAL:
                                        case NATIVE_UI_LAYOUT_SIZE_VERTICAL:
                                        case NATIVE_UI_LAYOUT_SIZE_HORIZONTAL:
                                                parent->update |= NATIVE_UI_UPDATE_SELF;
                                                parent->update |= NATIVE_UI_UPDATE_CHILDREN;
                                                parent = super_parent;
                                                super_parent = super_parent->parent;
                                                goto check_super_parent;
                                        default:
                                                break;
                                }
                        }
                }

        update:;
                parent->update |= NATIVE_UI_UPDATE_SELF;
                parent->update |= NATIVE_UI_UPDATE_CHILDREN;
                nview_update_layout(parent);
        } else {
                p->update |= NATIVE_UI_UPDATE_SELF;
                p->update |= NATIVE_UI_UPDATE_CHILDREN;
                nview_update_layout(p);
        }
}

void nview_request_position(struct nview *p, union vec2 position)
{
        nview_set_position(p, position);
        nview_request_layout(p);
}

void nview_request_size(struct nview *p, union vec2 size)
{
        nview_set_size(p, size);
        nview_request_layout(p);
}

void nview_request_fixed_size(struct nview *p, union vec2 size)
{
        nview_set_fixed_size(p, size);
        nview_request_layout(p);
}

void nview_request_scale(struct nview *p, union vec2 scale)
{
        nview_set_scale(p, scale);
        nview_request_layout(p);
}

void nview_request_rotation(struct nview *p, union vec3 rotation)
{
        nview_set_rotation(p, rotation);
        nview_request_layout(p);
}

void nview_request_margin(struct nview *p, union vec4 margin)
{
        p->align->margin        = margin;
        nview_request_layout(p);
}

void nview_request_padding(struct nview *p, union vec4 padding)
{
        p->align->padding              = padding;
        nview_request_layout(p);
}

void nview_set_layout_type(struct nview *p, u8 type)
{
        /*
         * ensure p has controller
         */
        if(list_singular(&p->layout_controller)) {
                struct nlyt_exec *controller = nlyt_exec_alloc();
                list_add_tail(&controller->head, &p->layout_controller);
        }
        /*
         * reset controller type
         */
        struct nlyt_exec *controller = (struct nlyt_exec *)
                ((char *)p->layout_controller.next - offsetof(struct nlyt_exec, head));
        if(controller->type != type) {
                p->update = NATIVE_UI_UPDATE_SELF | NATIVE_UI_UPDATE_CHILDREN;
        }
        controller->type        = type;
}

void nview_set_fixed_size(struct nview *p, union vec2 size)
{
        p->align->fixed_width   = size.width;
        p->align->fixed_height  = size.height;
}

void nview_touch_began(struct nview *p, union vec2 liv)
{
        p->touch_began_point = liv;
        p->touch_moved_point = liv;
        p->touch_offset      = (union vec2){0, 0};
        switch (p->type) {
                case NATIVE_UI_LISTVIEW:
                        nview_list_view_touch_began(p, liv);
                        break;
                default:
                        break;
        }
        if(p->touch_began_delegate) {
                p->touch_began_delegate(p, 0, liv);
        }
        if(p->parser_touch_handle) {
                ntouch_run(p->parser_touch_handle, p, NATIVE_UI_TOUCH_BEGAN);
        }
}

static struct nview *__find_intercept_vertical_view(struct nview *p)
{
        struct nview *target = p;

search:;
        if(!list_singular(&target->intercept_vertical)) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)target->intercept_vertical.next - offsetof(struct intercept_head, head));
                return ih->view;
        }

        if(!list_singular(&target->intercept_horizontal)) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)target->intercept_horizontal.next - offsetof(struct intercept_head, head));
                target = ih->view;
                goto search;
        }

        return NULL;
}

static struct nview *__find_intercept_horizontal_view(struct nview *p)
{
        struct nview *target = p;

search:;
        if(!list_singular(&target->intercept_horizontal)) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)target->intercept_horizontal.next - offsetof(struct intercept_head, head));
                return ih->view;
        }

        if(!list_singular(&target->intercept_vertical)) {
                struct intercept_head *ih = (struct intercept_head *)
                        ((char *)target->intercept_vertical.next - offsetof(struct intercept_head, head));
                target = ih->view;
                goto search;
        }
        return NULL;
}

void nview_touch_moved(struct nview *p, union vec2 liv)
{
        p->touch_offset = vec2_sub(liv, p->touch_moved_point);
        p->touch_moved_point = liv;

        if(p->intercept_vertical_target) {
                nview_touch_moved(p->intercept_vertical_target,
                        vec2_add(p->intercept_vertical_target->touch_began_point,
                                vec2_sub(p->touch_moved_point, p->touch_began_point)));
        } else if(p->intercept_horizontal_target){
                nview_touch_moved(p->intercept_horizontal_target,
                        vec2_add(p->intercept_horizontal_target->touch_began_point,
                                vec2_sub(p->touch_moved_point, p->touch_began_point)));
        } else {
                switch (p->type) {
                        case NATIVE_UI_LISTVIEW:
                                nview_list_view_touch_moved(p, liv);
                                break;
                        default:
                                break;
                }
                if(p->touch_moved_delegate) {
                        p->touch_moved_delegate(p, 0, liv);
                }
                if(p->parser_touch_handle) {
                        ntouch_run(p->parser_touch_handle, p, NATIVE_UI_TOUCH_MOVED);
                }
                if(!list_singular(&p->intercept_vertical) || !list_singular(&p->intercept_horizontal)) {
                        if(fabsf(p->touch_moved_point.x - p->touch_began_point.x) <= 10
                                && fabsf(p->touch_moved_point.y - p->touch_began_point.y) >= 10) {
                                struct nview *target = __find_intercept_vertical_view(p);
                                if(target) {
                                        nview_touch_cancelled(p, liv);
                                        p->intercept_vertical_target    = target;
                                        nview_touch_began(target, nview_convert_point_to_view(p, p->touch_began_point, target));
                                        goto finish;
                                }
                        }
                        else if(fabsf(p->touch_moved_point.x - p->touch_began_point.x) >= 10
                                && fabsf(p->touch_moved_point.y - p->touch_began_point.y) <= 10) {
                                struct nview *target = __find_intercept_horizontal_view(p);
                                if(target) {
                                        nview_touch_cancelled(p, liv);
                                        p->intercept_horizontal_target    = target;
                                        nview_touch_began(target, nview_convert_point_to_view(p, p->touch_began_point, target));
                                        goto finish;
                                }
                        }
                }
        }
finish:;
}

void nview_touch_ended(struct nview *p, union vec2 liv)
{
        p->touch_offset = vec2_sub(liv, p->touch_moved_point);
        p->touch_ended_point = liv;

        struct nview *intercept_vertical_target = p->intercept_vertical_target;
        struct nview *intercept_horizontal_target = p->intercept_horizontal_target;
        p->intercept_vertical_target    = NULL;
        p->intercept_horizontal_target  = NULL;

        if(intercept_vertical_target) {
                nview_touch_ended(intercept_vertical_target,
                        vec2_add(intercept_vertical_target->touch_began_point,
                                vec2_sub(p->touch_ended_point, p->touch_began_point)));
        } else if(intercept_horizontal_target){
                nview_touch_ended(intercept_horizontal_target,
                        vec2_add(intercept_horizontal_target->touch_began_point,
                                vec2_sub(p->touch_ended_point, p->touch_began_point)));
        } else {
                switch (p->type) {
                        case NATIVE_UI_LISTVIEW:
                                nview_list_view_touch_ended(p, liv);
                                break;
                        default:
                                break;
                }
                if(p->touch_ended_delegate) {
                        p->touch_ended_delegate(p, 0, liv);
                }
                if(p->parser_touch_handle) {
                        ntouch_run(p->parser_touch_handle, p, NATIVE_UI_TOUCH_ENDED);
                }
        }
}

void nview_touch_cancelled(struct nview *p, union vec2 liv)
{
        p->touch_offset = vec2_sub(liv, p->touch_moved_point);
        p->touch_ended_point = liv;

        struct nview *intercept_vertical_target = p->intercept_vertical_target;
        struct nview *intercept_horizontal_target = p->intercept_horizontal_target;
        p->intercept_vertical_target    = NULL;
        p->intercept_horizontal_target  = NULL;

        if(intercept_vertical_target) {
                nview_touch_cancelled(intercept_vertical_target,
                        vec2_add(intercept_vertical_target->touch_began_point,
                                vec2_sub(p->touch_ended_point, p->touch_began_point)));
        } else if(intercept_horizontal_target){
                nview_touch_cancelled(intercept_horizontal_target,
                        vec2_add(intercept_horizontal_target->touch_began_point,
                                vec2_sub(p->touch_ended_point, p->touch_began_point)));
        } else {
                p->touch_ended_point = liv;
                switch (p->type) {
                        case NATIVE_UI_LISTVIEW:
                                nview_list_view_touch_cancelled(p, liv);
                                break;
                        default:
                                break;
                }
                if(p->touch_cancelled_delegate) {
                        p->touch_cancelled_delegate(p, 0, liv);
                }
                if(p->parser_touch_handle) {
                        ntouch_run(p->parser_touch_handle, p, NATIVE_UI_TOUCH_CANCELLED);
                }
        }
}

void nview_text_done(struct nview *p)
{
        if(p->text_done_delegate) {
                p->text_done_delegate(p);
        }
}

void nview_set_touch_began_delegate(struct nview *p, nexec_tbeganf delegate)
{
        p->touch_began_delegate         = delegate;
}

void nview_set_touch_moved_delegate(struct nview *p, nexec_tmovedf delegate)
{
        p->touch_moved_delegate         = delegate;
}
void nview_set_touch_ended_delegate(struct nview *p, nexec_tendedf delegate)
{
        p->touch_ended_delegate         = delegate;
}

void nview_set_touch_cancelled_delegate(struct nview *p, nexec_tcancelledf delegate)
{
        p->touch_cancelled_delegate     = delegate;
}

void nview_set_text_done_delegate(struct nview *p, nexec_txtf delegate)
{
        p->text_done_delegate           = delegate;
}

void nview_clear_all_actions(struct nview *p)
{
        naction_key_clear(&p->action_key);
}

static void __nview_on_action_margin(struct nview *p)
{
        nview_request_margin(p, p->align->margin);
}

static void __nview_on_action_position(struct nview *p)
{
        nview_request_position(p, p->position);
}

static void __nview_on_action_scale(struct nview *p)
{
        nview_request_scale(p, p->scale);
}

static void __nview_on_action_rotation(struct nview *p)
{
        nview_request_rotation(p, p->rotation);
}

static void __nview_on_action_alpha(struct nview *p)
{
        nview_set_alpha(p, p->alpha);
}

struct naction *nview_margin_to(struct nview *p, union vec4 v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->align->margin,
                vec4_sub(v, p->align->margin),
                duration, ease, repeat, (nactionf)__nview_on_action_margin, p);
}

struct naction *nview_margin_by(struct nview *p, union vec4 v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->align->margin, v,
                duration, ease, repeat, (nactionf)__nview_on_action_margin, p);
}

struct naction *nview_move_to(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->position_expanded,
                vec4_sub((union vec4){v.x, v.y, 0.0, 0.0}, p->position_expanded),
                duration, ease, repeat, (nactionf)__nview_on_action_position, p);
}

struct naction *nview_move_by(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->position_expanded, (union vec4){v.x, v.y, 0.0, 0.0},
                duration, ease, repeat, (nactionf)__nview_on_action_position, p);
}

struct naction *nview_scale_to(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->scale_expanded,
                vec4_sub((union vec4){v.x, v.y, 0.0, 0.0}, p->scale_expanded),
                duration, ease, repeat, (nactionf)__nview_on_action_scale, p);
}

struct naction *nview_scale_by(struct nview *p, union vec2 v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->scale_expanded, (union vec4){v.x, v.y, 0.0, 0.0},
                duration, ease, repeat, (nactionf)__nview_on_action_scale, p);
}

struct naction *nview_rotate_by(struct nview *p, union vec3 v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->rotation_expanded, (union vec4){v.x, v.y, v.z, 0.0},
                duration, ease, repeat, (nactionf)__nview_on_action_rotation, p);
}

struct naction *nview_alpha_to(struct nview *p, float v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->alpha_expanded,
                vec4_sub((union vec4){v, 0.0, 0.0, 0.0}, p->alpha_expanded),
                duration, ease, repeat, (nactionf)__nview_on_action_alpha, p);
}

struct naction *nview_alpha_by(struct nview *p, float v, float duration, u8 ease, i16 repeat)
{
        return naction_alloc(&p->alpha_expanded, (union vec4){v, 0.0, 0.0, 0.0},
                duration, ease, repeat, (nactionf)__nview_on_action_alpha, p);
}

struct naction *nview_delay(struct nview *p, float duration)
{
        return naction_alloc_delay(&p->position_expanded, duration, NULL, NULL);
}

static void __show(struct nview *p)
{
        nview_set_visible(p, 1);
}

static void __hide(struct nview *p)
{
        nview_set_visible(p, 0);
}

struct naction *nview_show(struct nview *p)
{
        return nview_callback(p, __show, p);
}

struct naction *nview_hide(struct nview *p)
{
        return nview_callback(p, __hide, p);
}

struct naction *nview_callback(struct nview *p, nactionf callback, void *callback_data)
{
        return naction_alloc_callback(&p->position_expanded, callback, callback_data);
}

void nview_run_action(struct nview *p, ...)
{
        va_list parg;
        va_start(parg, p);
        struct naction *a = va_arg(parg, struct naction *);
        while(a) {
                naction_key_add_action(&p->action_key, a);
                a = va_arg(parg, struct naction *);
        }
        va_end(parg);

        nmanager_add_action_key(nmanager_shared(), &p->action_key);
}

union vec2 nview_get_screen_pos(struct nview *p, union vec2 point)
{
        struct nview *r = p;
        while(r) {
                point.x += r->position.x - r->anchor.x * r->size.width;
                point.y += r->position.y - r->anchor.y * r->size.height;
                r = r->parent;
        }
        return point;
}

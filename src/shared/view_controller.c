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
#include <native_ui/view_controller.h>
#include <native_ui/view.h>
#include <native_ui/manager.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/stdio.h>
#include <cherry/pool_list.h>
#include <cherry/map.h>

static native_controller_afnf __from_name_delegate = NULL;

struct native_controller *native_controller_alloc_fn(char *name, size_t len)
{
        if(__from_name_delegate) return __from_name_delegate(name, len);
        else return NULL;
}

void native_controller_set_fnf(native_controller_afnf delegate)
{
        __from_name_delegate = delegate;
}

static void __free_task(struct list_head *head)
{
        if(!list_singular(head)) {
                struct native_task *task = (struct native_task *)
                        ((char *)head->next - offsetof(struct native_task, user_head));
                native_task_free(task);
        }
        sfree(head);
}

struct native_controller *native_controller_alloc()
{
        struct native_controller *p        = smalloc(sizeof(struct native_controller), native_controller_free);
        p->custom_data                          = NULL;
        p->custom_data_free                     = NULL;
        p->parent                               = NULL;
        p->on_linked                            = NULL;
        p->on_removed                           = NULL;
        p->on_update                            = NULL;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->view);
        INIT_POOL_LIST_HEAD(&p->anonymous);

        p->tasks                                = map_alloc(sizeof(struct list_head *));

        return p;
}

void native_controller_free(struct native_controller *p)
{
        /*
         * free update task
         */
        map_deep_free(p->tasks, struct list_head *, __free_task);
        struct pool_list_head *plhead;
        while(!pool_list_singular(&p->anonymous)) {
                plhead = pool_list_get(&p->anonymous);
                pool_list_del(plhead);
        }

        if(!list_singular(&p->view)) {
                struct native_view *view = (struct native_view *)
                        ((char *)p->view.next - offsetof(struct native_view, view_controller));
                list_del_init(&p->view);
                debug("native ui controller free view\n");
                native_view_free(view);
        }
        struct list_head *head;
        list_while_not_singular(head, &p->children) {
                struct native_controller *child = (struct native_controller *)
                        ((char *)head - offsetof(struct native_controller, head));
                list_del_init(head);
                if(child->on_removed) {
                        child->on_removed(child);
                }
                native_controller_free(child);
        }
        if(!list_singular(&p->head) && p->on_removed) {
                list_del_init(&p->head);
                p->on_removed(p);
        } else {
                list_del_init(&p->head);
        }
        if(p->custom_data && p->custom_data_free) {
                p->custom_data_free(p->custom_data);
                p->custom_data          = NULL;
                p->custom_data_free     = NULL;
        }
        sfree(p);
}

void native_controller_register_task(struct native_controller *p, char *key, size_t len, int count, native_task_delegate delegate)
{
        if(map_has_key(p->tasks, key, len)) {
                struct list_head *head = map_get(p->tasks, struct list_head *, key, len);
                __free_task(head);
                map_remove_key(p->tasks, key, len);
        }

        struct list_head *head   = smalloc(sizeof(struct list_head), NULL);
        INIT_LIST_HEAD(head);

        struct native_task *task = native_task_alloc();
        list_add_tail(&task->user_head, head);
        task->count     = count;
        task->data      = p;
        task->delegate  = delegate;

        map_set(p->tasks, key, len, &head);
}

void native_controller_unreg(struct native_controller *p, char *key, size_t len)
{
        if(map_has_key(p->tasks, key, len)) {
                struct list_head *head = map_get(p->tasks, struct list_head *, key, len);
                __free_task(head);
                map_remove_key(p->tasks, key, len);
        }
}

void native_controller_link(struct native_controller *p, struct native_controller *c)
{
        if(!list_singular(&c->head) && c->on_removed) {
                list_del_init(&c->head);
                c->on_removed(c);
        } else {
                list_del_init(&c->head);
        }
        list_add_tail(&c->head, &p->children);
        c->parent       = p;

        if(c->on_linked) {
                c->on_linked(c);
        }
}

struct native_view *native_controller_get_view(struct native_controller *p)
{
        if(!list_singular(&p->view)) {
                struct native_view *view = (struct native_view *)
                        ((char *)p->view.next - offsetof(struct native_view, view_controller));
                return view;
        }
        return NULL;
}

void native_controller_set_view(struct native_controller *p, struct native_view *v)
{
        if(!list_singular(&v->view_controller)) {
                struct native_controller *controller = (struct native_controller *)
                        ((char *)v->view_controller.next - offsetof(struct native_controller, view));
                list_del_init(&v->view_controller);
                native_controller_free(controller);
        }
        if(!list_singular(&p->view)) {
                struct native_view *view = (struct native_view *)
                        ((char *)p->view.next - offsetof(struct native_view, view_controller));
                list_del_init(&p->view);
                native_view_free(view);
        }
        list_add_tail(&v->view_controller, &p->view);
}

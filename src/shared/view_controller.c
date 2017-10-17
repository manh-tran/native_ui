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
#include <cherry/map.h>

static nexec_afnf __from_name_delegate = NULL;

struct nexec *nexec_alloc_fn(char *name, size_t len)
{
        if(__from_name_delegate) return __from_name_delegate(name, len);
        else return NULL;
}

void nexec_set_fnf(nexec_afnf delegate)
{
        __from_name_delegate = delegate;
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

struct nexec *nexec_alloc()
{
        struct nexec *p        = smalloc(sizeof(struct nexec), nexec_free);
        p->custom_data                          = NULL;
        p->custom_data_free                     = NULL;
        p->parent                               = NULL;
        p->on_linked                            = NULL;
        p->on_removed                           = NULL;
        p->on_update                            = NULL;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->view);

        p->tasks                                = map_alloc(sizeof(struct list_head *));

        return p;
}

void nexec_free(struct nexec *p)
{
        /*
         * free update task
         */
        map_deep_free(p->tasks, struct list_head *, __free_task);

        if(!list_singular(&p->view)) {
                struct nview *view = (struct nview *)
                        ((char *)p->view.next - offsetof(struct nview, view_controller));
                list_del_init(&p->view);
                debug("native ui controller free view\n");
                nview_free(view);
        }
        struct list_head *head;
        list_while_not_singular(head, &p->children) {
                struct nexec *child = (struct nexec *)
                        ((char *)head - offsetof(struct nexec, head));
                list_del_init(head);
                if(child->on_removed) {
                        child->on_removed(child);
                }
                nexec_free(child);
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

void nexec_reg(struct nexec *p, char *key, size_t len, int count, ntaskf delegate)
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

void nexec_unreg(struct nexec *p, char *key, size_t len)
{
        if(map_has_key(p->tasks, key, len)) {
                struct list_head *head = map_get(p->tasks, struct list_head *, key, len);
                __free_task(head);
                map_remove_key(p->tasks, key, len);
        }
}

void nexec_link(struct nexec *p, struct nexec *c)
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

struct nview *nexec_get_view(struct nexec *p)
{
        if(!list_singular(&p->view)) {
                struct nview *view = (struct nview *)
                        ((char *)p->view.next - offsetof(struct nview, view_controller));
                return view;
        }
        return NULL;
}

void nexec_set_view(struct nexec *p, struct nview *v)
{
        if(!list_singular(&v->view_controller)) {
                struct nexec *controller = (struct nexec *)
                        ((char *)v->view_controller.next - offsetof(struct nexec, view));
                list_del_init(&v->view_controller);
                nexec_free(controller);
        }
        if(!list_singular(&p->view)) {
                struct nview *view = (struct nview *)
                        ((char *)p->view.next - offsetof(struct nview, view_controller));
                list_del_init(&p->view);
                nview_free(view);
        }
        list_add_tail(&v->view_controller, &p->view);
}

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
#include <native_ui/manager.h>
#include <native_ui/action.h>
#include <native_ui/touch.h>
#include <native_ui/view.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/stdio.h>
#include <cherry/math/math.h>
#include <pthread.h>

struct native_task *native_task_alloc()
{
        struct native_task *p = smalloc(sizeof(struct native_task), native_task_free);
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->user_head);
        p->data                         = NULL;
        p->delegate                     = NULL;
        p->count                        = 0;

        struct native_manager *manager = native_manager_shared();
        list_add_tail(&p->head, &manager->update_tasks);

        return p;
}

void native_task_free(struct native_task *p)
{
        list_del(&p->head);
        list_del(&p->user_head);
        sfree(p);
}

/*
 * native ui manager
 */
static struct native_manager *__manager = NULL;

static void __clear()
{
        if(__manager) {
                native_manager_free(__manager);
                __manager = NULL;
        }
}

static void __setup()
{
        if(!__manager) {
                cache_add(__clear);
                __manager = native_manager_alloc();
        }
}

struct native_manager *native_manager_shared()
{
        __setup();
        return __manager;
}

struct native_manager *native_manager_alloc()
{
        struct native_manager *p     = smalloc(sizeof(struct native_manager), native_manager_free);
        INIT_LIST_HEAD(&p->update_tasks);
        INIT_LIST_HEAD(&p->action_keys);
        INIT_LIST_HEAD(&p->touches);
        return p;
}

void native_manager_free(struct native_manager *p)
{
        struct list_head *head;
        list_for_each_secure_mutex_lock(head, &p->touches, &__shared_native_view_touch_lock, {
                struct native_view_touch_data *data = (struct native_view_touch_data *)
                        ((char *)head - offsetof(struct native_view_touch_data, shared_head));
                native_view_touch_data_free(data);
        });
        list_while_not_singular(head, &p->update_tasks) {
                struct native_task *task      = (struct native_task *)
                        ((char *)head - offsetof(struct native_task, head));
                native_task_free(task);
        }
        list_while_not_singular(head, &p->action_keys) {
                struct naction_key *key = (struct naction_key *)
                        ((char *)head - offsetof(struct naction_key, key_head));
                naction_key_clear(key);
        }
        sfree(p);
}

void native_manager_add_action_key(struct native_manager *p, struct naction_key *key)
{
        list_del(&key->key_head);
        list_add_tail(&key->key_head, &p->action_keys);
}

static void __native_manager_update_action(struct native_manager *p, float delta)
{
        struct list_head *head, *next;
        list_for_each_secure(head, &p->action_keys, {
                struct naction_key *key = (struct naction_key *)
                        ((char *)head - offsetof(struct naction_key, key_head));
                u8 all_finish   = 1;
                struct list_head *action_head;

                list_for_each_secure(action_head, &key->actions, {
                        struct naction *a = (struct naction *)
                                ((char *)action_head - offsetof(struct naction, head));
                        u8 finish = naction_update(a, delta);
                        if(finish) {
                                naction_free(a);
                        }
                        all_finish &= finish;
                });
                if(all_finish) {
                        list_del_init(&key->key_head);
                }
        });
}

static void __native_manager_update_task(struct native_manager *p, float delta)
{
        struct list_head *head;
        list_for_each_secure(head, &p->update_tasks, {
                struct native_task *task      = (struct native_task *)
                        ((char *)head - offsetof(struct native_task, head));
                if(task->delegate) {
                        task->delegate(task->data, delta);
                }
                if(task->count > 0) {
                        task->count--;
                }
                if(task->count == 0) {
                        native_task_free(task);
                }
        });
}

static void __native_manager_update_touch(struct native_manager *p)
{
        struct list_head *head;
        list_for_each_secure_mutex_lock(head, &p->touches, &__shared_native_view_touch_lock, {
                struct native_view_touch_data *data = (struct native_view_touch_data *)
                        ((char *)head - offsetof(struct native_view_touch_data, shared_head));
                struct native_view *view        = data->view;
                u8 type                         = data->type;
                union vec2 pt = data->point;

                native_view_touch_data_free(data);

                switch (type) {
                        case NATIVE_UI_TOUCH_BEGAN:
                                native_view_touch_began(view, pt);
                                break;
                        case NATIVE_UI_TOUCH_MOVED:
                                native_view_touch_moved(view, pt);
                                break;
                        case NATIVE_UI_TOUCH_ENDED:
                                native_view_touch_ended(view, pt);
                                break;
                        case NATIVE_UI_TOUCH_CANCELLED:
                                native_view_touch_cancelled(view, pt);
                                break;
                }
        });
}

void native_manager_update(struct native_manager *p, float delta)
{
        __native_manager_update_touch(p);
        __native_manager_update_task(p, delta);
        __native_manager_update_action(p, delta);
}

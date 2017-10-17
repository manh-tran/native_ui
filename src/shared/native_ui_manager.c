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

struct ntask *ntask_alloc()
{
        struct ntask *p = smalloc(sizeof(struct ntask), ntask_free);
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->user_head);
        p->data                         = NULL;
        p->delegate                     = NULL;
        p->count                        = 0;

        struct nmanager *manager = nmanager_shared();
        list_add_tail(&p->head, &manager->update_tasks);

        return p;
}

void ntask_free(struct ntask *p)
{
        list_del(&p->head);
        list_del(&p->user_head);
        sfree(p);
}

/*
 * native ui manager
 */
static struct nmanager *__manager = NULL;

static void __clear()
{
        if(__manager) {
                nmanager_free(__manager);
                __manager = NULL;
        }
}

static void __setup()
{
        if(!__manager) {
                cache_add(__clear);
                __manager = nmanager_alloc();
        }
}

struct nmanager *nmanager_shared()
{
        __setup();
        return __manager;
}

struct nmanager *nmanager_alloc()
{
        struct nmanager *p     = smalloc(sizeof(struct nmanager), nmanager_free);
        INIT_LIST_HEAD(&p->update_tasks);
        INIT_LIST_HEAD(&p->action_keys);
        INIT_LIST_HEAD(&p->touches);
        return p;
}

void nmanager_free(struct nmanager *p)
{
        struct list_head *head;
        list_for_each_secure_mutex_lock(head, &p->touches, &__shared_nview_touch_lock, {
                struct nview_touch_data *data = (struct nview_touch_data *)
                        ((char *)head - offsetof(struct nview_touch_data, shared_head));
                nview_touch_data_free(data);
        });
        list_while_not_singular(head, &p->update_tasks) {
                struct ntask *task      = (struct ntask *)
                        ((char *)head - offsetof(struct ntask, head));
                ntask_free(task);
        }
        list_while_not_singular(head, &p->action_keys) {
                struct naction_key *key = (struct naction_key *)
                        ((char *)head - offsetof(struct naction_key, key_head));
                naction_key_clear(key);
        }
        sfree(p);
}

void nmanager_add_action_key(struct nmanager *p, struct naction_key *key)
{
        list_del(&key->key_head);
        list_add_tail(&key->key_head, &p->action_keys);
}

static void __nmanager_update_action(struct nmanager *p, float delta)
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

static void __nmanager_update_task(struct nmanager *p, float delta)
{
        struct list_head *head;
        list_for_each_secure(head, &p->update_tasks, {
                struct ntask *task      = (struct ntask *)
                        ((char *)head - offsetof(struct ntask, head));
                if(task->delegate) {
                        task->delegate(task->data, delta);
                }
                if(task->count > 0) {
                        task->count--;
                }
                if(task->count == 0) {
                        ntask_free(task);
                }
        });
}

static void __nmanager_update_touch(struct nmanager *p)
{
        struct list_head *head;
        list_for_each_secure_mutex_lock(head, &p->touches, &__shared_nview_touch_lock, {
                struct nview_touch_data *data = (struct nview_touch_data *)
                        ((char *)head - offsetof(struct nview_touch_data, shared_head));
                struct nview *view        = data->view;
                u8 type                         = data->type;
                union vec2 pt = data->point;

                nview_touch_data_free(data);

                switch (type) {
                        case NATIVE_UI_TOUCH_BEGAN:
                                nview_touch_began(view, pt);
                                break;
                        case NATIVE_UI_TOUCH_MOVED:
                                nview_touch_moved(view, pt);
                                break;
                        case NATIVE_UI_TOUCH_ENDED:
                                nview_touch_ended(view, pt);
                                break;
                        case NATIVE_UI_TOUCH_CANCELLED:
                                nview_touch_cancelled(view, pt);
                                break;
                }
        });
}

void nmanager_update(struct nmanager *p, float delta)
{
        __nmanager_update_touch(p);
        __nmanager_update_task(p, delta);
        __nmanager_update_action(p, delta);
}

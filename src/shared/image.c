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
#include <native_ui/image.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/string.h>

static struct map *__cache = NULL;

static void __clear()
{
        if(__cache) {
                map_deep_free(__cache, struct nimage *, nimage_free);
                __cache = NULL;
        }
}

static void __setup()
{
        if(!__cache) {
                __cache = map_alloc(sizeof(struct nimage *));
                cache_add(__clear);
        }
}
/*
 * image head
 */
struct image_head {
        struct list_head cache_head;

        struct list_head user_head;

        struct nimage  *image;
};

static void __image_head_free(struct image_head *p)
{
        list_del_init(&p->cache_head);
        list_del_init(&p->user_head);
        if(list_singular(&p->image->links)) {
                nimage_free(p->image);
        }
        sfree(p);
}

static struct image_head *__image_head_alloc(struct nimage *p)
{
        struct image_head *head = smalloc(sizeof(struct image_head), __image_head_free);
        INIT_LIST_HEAD(&head->cache_head);
        INIT_LIST_HEAD(&head->user_head);
        head->image     = p;
        return head;
}

void nimage_un_link(struct list_head *head)
{
        if(!list_singular(head)) {
                struct list_head *user_head = head->next;
                struct image_head *imh  = (struct image_head *)
                        ((char *)user_head - offsetof(struct image_head, user_head));
                __image_head_free(imh);
        }
        list_del_init(head);
}

void nimage_link_path(char *name, struct list_head *head)
{
        nimage_un_link(head);

        struct nimage *p = nimage_alloc(name);
        struct image_head *imh  = __image_head_alloc(p);
        list_add_tail(head, &imh->user_head);
        list_add_tail(&imh->cache_head, &p->links);
}

struct nimage *nimage_get_image(struct list_head *head)
{
        if(!list_singular(head)) {
                struct list_head *user_head = head->next;
                struct image_head *imh  = (struct image_head *)
                        ((char *)user_head - offsetof(struct image_head, user_head));
                return imh->image;
        }
        return NULL;
}

struct nimage *nimage_alloc(char *name)
{
        __setup();
        size_t len = strlen(name);
        /*
         * fetch image from cache
         */
        if(map_has_key(__cache, name, len)) {
                return map_get(__cache, struct nimage *, name, len);
        }

        struct nimage *p       = smalloc(sizeof(struct nimage), nimage_free);
        p->path                         = string_alloc_chars(name, len);
        INIT_LIST_HEAD(&p->links);
        nimage_init(p, name);
        /*
         * save image to cache
         */
        map_set(__cache, qskey(p->path), &p);
        return p;
}

void nimage_free_common(struct nimage *p)
{
        string_free(p->path);
        struct list_head *head;
        list_while_not_singular(head, &p->links) {
                struct image_head *h = (struct image_head *)
                        ((char *)head - offsetof(struct image_head, cache_head));
                list_del_init(&h->cache_head);
                list_del_init(&h->user_head);
                sfree(h);
        }
        map_remove_key(__cache, qskey(p->path));
}

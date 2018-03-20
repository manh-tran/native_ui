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
#include <native_ui/preferences.h>
#include <smartfox/data.h>
#include <cherry/map.h>
#include <cherry/array.h>
#include <cherry/string.h>
#include <cherry/stdio.h>
#include <cherry/memory.h>
#include <native_ui/file.h>

static void __p_free(struct npref *p)
{
        npref_save(p);
        sobj_free(p->data);
        string_free(p->key);
        sfree(p);
}


static struct npref *__p_alloc(char *content, size_t len, char *key, size_t key_len)
{
        struct npref *p = smalloc(sizeof(struct npref), __p_free);
        int counter = 0;
        p->data = sobj_from_json(content, len, &counter);
        p->key  = string_alloc_chars(key, key_len);

        return p;
}

static struct map *__cache = NULL;

static void __clear()
{
        if(__cache) {
                map_deep_free(__cache, struct npref *, __p_free);
        }
}

static void __setup()
{
        if(!__cache) {
                cache_add(__clear);

                __cache = map_alloc(sizeof(struct npref *));
        }
}

struct npref *npref_get(char *name, size_t len)
{
        __setup();
        if(!len) return NULL;

        struct string *key = string_alloc_chars(qlkey(LOCAL_PATH));
        if(name[0] == '/') {
                string_cat(key, name + 1, len - 1);
        } else {
                string_cat(key, name, len);
        }
        if( ! map_has_key(__cache, qskey(key))) {
                struct file *f = nfile_open(key->ptr, "rw");
                struct string *content = file_to_string(f);
                struct npref *pref = __p_alloc(qskey(content), qskey(key));
                map_set(__cache, qskey(key), &pref);
                string_free(content);
                file_close(f);
        }
        struct npref *p = map_get(__cache, struct npref *, qskey(key));
        string_free(key);
        return p;
}

void npref_save(struct npref *p)
{
        struct string *content = sobj_to_json(p->data);
        struct file *f = nfile_open(p->key->ptr, "w");
        debug("native ui file write %s %p\n", p->key->ptr, f->ptr);
        file_write(f, qskey(content));
        file_close(f);
        string_free(content);
}

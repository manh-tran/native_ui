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
#include <native_ui/font.h>
#include <cherry/memory.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>

static struct map *__font_cache = NULL;

static void __clear()
{
        if(__font_cache) {
                map_deep_free(__font_cache, struct native_ui_font *, native_ui_font_free);
                __font_cache = NULL;
        }
}

static void __setup()
{
        if(!__font_cache) {
                __font_cache = map_alloc(sizeof(struct native_ui_font *));
                cache_add(__clear);
        }
}

struct native_ui_font *native_ui_font_get(char *name, size_t len, float size)
{
        __setup();

        struct string *key = string_alloc_chars(name, len);
        string_cat_int(key, size);

        struct native_ui_font *p        = map_get(__font_cache, struct native_ui_font *, qskey(key));
        if(p) goto finish;

        p                               = smalloc(sizeof(struct native_ui_font), native_ui_font_free);
        p->name                         = string_alloc_chars(name, len);
        p->size                         = size;
        native_ui_font_init(p);

        map_set(__font_cache, qskey(key), &p);

finish:;
        string_free(key);
        return p;
}

void native_ui_font_free(struct native_ui_font *p)
{
        native_ui_font_free_platform(p);
        string_free(p->name);
        sfree(p);
}

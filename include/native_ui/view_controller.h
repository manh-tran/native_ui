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
#ifndef __NATIVE_UI_VIEW_CONTROLLER_H__
#define __NATIVE_UI_VIEW_CONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

typedef struct nexec *(*nexec_afnf)(char *, size_t);

struct nexec *nexec_alloc_fn(char *name, size_t len);

void nexec_set_fnf(nexec_afnf delegate);

struct nexec *nexec_alloc();

void nexec_free(struct nexec *p);

void nexec_link(struct nexec *p, struct nexec *c);

void nexec_set_view(struct nexec *p, struct nview *v);

struct nview *nexec_get_view(struct nexec *p);

void nexec_reg(struct nexec *p, char *key, size_t len, int count, ntaskf delegate);

void nexec_unreg(struct nexec *p, char *key, size_t len);


#ifdef __cplusplus
}
#endif

#endif

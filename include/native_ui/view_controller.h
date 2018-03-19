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

typedef struct native_controller *(*native_controller_afnf)(char *, size_t);

struct native_controller *native_controller_alloc_fn(char *name, size_t len);

void native_controller_set_fnf(native_controller_afnf delegate);

struct native_controller *native_controller_alloc();

void native_controller_free(struct native_controller *p);

void native_controller_link(struct native_controller *p, struct native_controller *c);

void native_controller_set_view(struct native_controller *p, struct native_view *v);

struct native_view *native_controller_get_view(struct native_controller *p);

void native_controller_register_task(struct native_controller *p, char *key, size_t len, int count, native_task_delegate delegate);

void native_controller_unreg(struct native_controller *p, char *key, size_t len);


#ifdef __cplusplus
}
#endif

#endif

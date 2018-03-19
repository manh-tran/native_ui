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
#ifndef __NATIVE_UI_VIEW_LAYOUT_CONTROLLER_H__
#define __NATIVE_UI_VIEW_LAYOUT_CONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

struct native_layout *native_layout_alloc();

void native_layout_free(struct native_layout *p);

void native_layout_update(struct native_layout *p);

#ifdef __cplusplus
}
#endif

#endif

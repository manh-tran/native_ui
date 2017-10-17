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
#ifndef __NATIVE_UI_SHARED_LAYOUT_CONTROLLER_H__
#define __NATIVE_UI_SHARED_LAYOUT_CONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

void nlyt_exec_update_absolute(struct nlyt_exec *p);

void nlyt_exec_update_relative(struct nlyt_exec *p);

void nlyt_exec_update_vertical(struct nlyt_exec *p);

void nlyt_exec_update_horizontal(struct nlyt_exec *p);

void nlyt_exec_update_size_vertical(struct nlyt_exec *p);

void nlyt_exec_update_size_horizontal(struct nlyt_exec *p);

#ifdef __cplusplus
}
#endif

#endif

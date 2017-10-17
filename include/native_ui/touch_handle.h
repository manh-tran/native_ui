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
#ifndef __NATIVE_UI_TOUCH_HANDLE_H__
#define __NATIVE_UI_TOUCH_HANDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

struct ntouch *ntouch_alloc();

void ntouch_free(struct ntouch *p);

void ntouch_set_f(struct ntouch *p, ntouchf delegate, void *custom_data, custom_data_free_delegate free_delegate);

void ntouch_run(struct ntouch *p, void *sender, u8 type);

void ntouch_link(struct ntouch *p, struct ntouch *c);

#ifdef __cplusplus
}
#endif

#endif

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
#ifndef __NATIVE_UI_IMAGE_H__
#define __NATIVE_UI_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

struct nimage *nimage_alloc(char *name);

void nimage_init(struct nimage *p, char *name);

void nimage_free(struct nimage *p);

void nimage_free_common(struct nimage *p);

void nimage_link_path(char *name, struct list_head *head);

void nimage_un_link(struct list_head *head);

struct nimage *nimage_get_image(struct list_head *link);

#ifdef __cplusplus
}
#endif

#endif

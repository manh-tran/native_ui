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
#ifndef __NATIVE_UI_VIEW_DATA_H__
#define __NATIVE_UI_VIEW_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

struct nview_image_view_data *image_view_data_alloc(char *path);

void image_view_data_free(struct nview_image_view_data *p);

struct nview_list_view_data *nview_list_view_data_alloc();

void nview_list_view_data_free(struct nview_list_view_data *p);

struct nview_label_data *nview_label_data_alloc();

void nview_label_data_free(struct nview_label_data *p);

#ifdef __cplusplus
}
#endif

#endif

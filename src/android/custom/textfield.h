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
#ifndef __native_view_ANDROID_CUSTOM_TEXT_FIELD_H__
#define __native_view_ANDROID_CUSTOM_TEXT_FIELD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

jobject custom_text_field_alloc(void *np);

struct string *custom_text_field_get_text(jobject p);

void custom_text_field_set_placeholder(jobject p, char *text, size_t len);

#ifdef __cplusplus
}
#endif

#endif

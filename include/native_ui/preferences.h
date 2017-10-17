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
#ifndef __npref_H__
#define __npref_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

struct npref *npref_get(char *name, size_t len);

void npref_save(struct npref *p);

#ifdef __cplusplus
}
#endif

#endif

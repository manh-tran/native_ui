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
#ifndef __NATIVE_UI_NATIVE_UI_MANAGER_H__
#define __NATIVE_UI_NATIVE_UI_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

/*
 * update task
 */
struct ntask *ntask_alloc();

void ntask_free(struct ntask *p);

/*
 * manager
 */
struct nmanager *nmanager_shared();

struct nmanager *nmanager_alloc();

void nmanager_free(struct nmanager *p);

void nmanager_update(struct nmanager *p, float delta);

void nmanager_add_action_key(struct nmanager *p, struct naction_key *key);

#ifdef __cplusplus
}
#endif

#endif

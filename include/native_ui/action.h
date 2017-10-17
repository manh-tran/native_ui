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
#ifndef __NATIVE_UI_ACTION_H__
#define __NATIVE_UI_ACTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <native_ui/types.h>

/*
 * action key manipulations
 */
void naction_key_init(struct naction_key *p);

void naction_key_clear(struct naction_key *p);

void naction_key_add_action(struct naction_key *p, struct naction *a);

/*
 * acion manipulations
 */
struct naction *naction_alloc(union vec4 *target, union vec4 offset, float duration, u8 type, i16 repeat, nactionf callback, void *callback_data);

struct naction *naction_alloc_force(union vec4 *target, union vec4 destination, nactionf callback, void *callback_data);

struct naction *naction_alloc_gravity(union vec4 *target, float velocity, float max_velocity, float accelerate, nactionf callback, void *callback_data, ...);

struct naction *naction_alloc_delay(union vec4 *target, float duration, nactionf callback, void *callback_data);

struct naction *naction_alloc_callback(union vec4 *target, nactionf callback, void *callback_data);

void naction_free(struct naction *p);

u8 naction_update(struct naction *p, float delta);

struct naction *naction_sequence(struct naction *p, ...);

struct naction *naction_repeat(int count, struct naction *p, ...);

struct naction *naction_parallel(struct naction *p, ...);

#ifdef __cplusplus
}
#endif

#endif

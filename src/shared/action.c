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
#include <native_ui/action.h>
#include <cherry/list.h>
#include <cherry/memory.h>
#include <cherry/math/math.h>
#include <cherry/array.h>
#include <cherry/stdio.h>

static inline void __action_reset(struct naction *p)
{
        p->finish               = 0;
        switch (p->ease_type) {
                case NATIVE_UI_EASE_DELAY:
                        p->delay                = p->duration;
                        break;
                default:
                        p->remain               = p->duration;
                        p->last_amount_offset   = (union vec4){0, 0, 0, 0};
                         break;
        }

        struct list_head *head;
        list_for_each(head, &p->children) {
                struct naction *child = (struct naction *)
                        ((char *)head - offsetof(struct naction, head));
                __action_reset(child);
        }
}

static struct naction *__action_alloc_sequence()
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = NATIVE_UI_EASE_SEQUENCE;
        p->finish               = 0;
        p->repeat               = 0;
        p->callback             = NULL;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

static struct naction *__action_alloc_repeat(int count)
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = NATIVE_UI_EASE_SEQUENCE;
        p->finish               = 0;
        p->repeat               = count;
        p->callback             = NULL;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

static struct naction *__action_alloc_parallel()
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = NATIVE_UI_EASE_PARALLEL;
        p->finish               = 0;
        p->repeat               = 0;
        p->callback             = NULL;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}


struct naction *naction_alloc(union vec4 *target, union vec4 offset, float duration,
        u8 type, i16 repeat, nactionf callback, void *callback_data)
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = type;
        p->target               = target;
        p->offset               = offset;
        p->last_amount_offset   = (union vec4){0, 0, 0, 0};
        p->duration             = duration;
        p->remain               = duration;
        p->finish               = 0;
        p->repeat               = repeat;
        p->callback             = callback;
        p->callback_data        = callback_data;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

struct naction *naction_alloc_force(union vec4 *target, union vec4 destination,
        nactionf callback, void *callback_data)
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = NATIVE_UI_EASE_FORCE;
        p->target               = target;
        p->destination          = destination;
        p->finish               = 0;
        p->repeat               = 0;
        p->callback             = callback;
        p->callback_data        = callback_data;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

struct naction *naction_alloc_gravity(union vec4 *target, float velocity, float max_velocity,
        float accelerate, nactionf callback, void *callback_data, ...)
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = NATIVE_UI_EASE_GRAVITY;
        p->target               = target;
        p->velocity             = velocity;
        p->max_velocity         = max_velocity;
        p->accelerate           = accelerate;
        p->finish               = 0;
        p->repeat               = 0;
        p->destinations         = array_alloc(sizeof(union vec4), ORDERED);
        p->index                = 0;
        p->callback             = callback;
        p->callback_data        = callback_data;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        va_list parg;
        va_start(parg, callback_data);
        union vec4 *v = va_arg(parg, union vec4 *);
        while(v) {
                array_push(p->destinations, v);
                v = va_arg(parg, union vec4 *);
        }
        va_end(parg);

        return p;
}

struct naction *naction_alloc_delay(union vec4 *target, float duration,
        nactionf callback, void *callback_data)
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = NATIVE_UI_EASE_DELAY;
        p->target               = target;
        p->delay                = duration;
        p->duration       = duration;
        p->finish               = 0;
        p->repeat               = 0;
        p->callback             = callback;
        p->callback_data        = callback_data;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

struct naction *naction_alloc_callback(union vec4 *target,
        nactionf callback, void *callback_data)
{
        struct naction *p        = smalloc(sizeof(struct naction), naction_free);
        p->ease_type            = NATIVE_UI_EASE_FORCE;
        p->target               = target;
        p->callback             = callback;
        p->callback_data        = callback_data;
        p->finish               = 0;
        p->repeat               = 0;
        INIT_LIST_HEAD(&p->head);
        INIT_LIST_HEAD(&p->children);
        INIT_LIST_HEAD(&p->user_head);
        return p;
}

void naction_free(struct naction *p)
{
        struct list_head *head;
        list_while_not_singular(head, &p->children) {
                struct naction *child = (struct naction *)
                        ((char *)head - offsetof(struct naction, head));
                naction_free(child);
        }
        list_del(&p->head);
        list_del(&p->user_head);
        if(p->ease_type == NATIVE_UI_EASE_GRAVITY) {
                array_free(p->destinations);
        }
        sfree(p);
}

static inline void __action_update_target(struct naction *p, float percent)
{
        *p->target              = vec4_sub(*p->target, p->last_amount_offset);
        p->last_amount_offset   = vec4_mul_scalar(p->offset, percent);
        *p->target              = vec4_add(*p->target, p->last_amount_offset);
        if(p->remain <= EPSILON) {
                p->finish = 1;
                p->remain = 0.0f;
        }
}

static void __action_ease_linear(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quadratic_in(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quadratic_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * (2 - t);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quadratic_in_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * 0.5f;
                } else {
                        t = t - 1;
                        t = -0.5f * (t * (t - 2) - 1);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_cubic_in(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_cubic_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = (t * t * t + 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_cubic_in_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * t * 0.5;
                } else {
                        t = t - 2;
                        t = (t * t * t + 2) / 2;
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quartic_in(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t * t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quartic_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = -(t * t * t * t - 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quartic_in_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * t * t * 0.5f;
                } else {
                        t = t - 2;
                        t = -0.5f * (t * t * t * t - 2);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quintic_in(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t * t * t * t * t;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quintic_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = (t * t * t * t * t + 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_quintic_in_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = t * t * t * t * t * 0.5f;
                } else {
                        t = t - 2;
                        t = (t * t * t * t * t + 2) * 0.5f;
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_sinusoidal_in(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -cos(t * M_PI * 0.5f) + 1;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_sinusoidal_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = sin(t * M_PI * 0.5f);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_sinusoidal_in_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -0.5f * (cos(t * M_PI) - 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_exponential_in(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = (float)pow(2.0, 10.0 * (t - 1));
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_exponential_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -(float)pow(2.0, -10.0 * t) + 1;
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_exponential_in_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = 0.5f * (float)pow(2.0, 10.0 * (t - 1));
                } else {
                        t = t - 1;
                        t = 0.5f * (-(float)pow(2.0, -10.0 * t) + 2);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_circular_in(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = -((float)sqrt(1.0 - t * t) - 1);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_circular_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = (p->duration - remain) / p->duration;
                t = t - 1;
                t = (float)sqrt(1.0 - t * t);
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_circular_in_out(struct naction *p, float delta)
{
        float t         = 0.0f;
        float remain    = p->remain - delta;
        if(remain >= EPSILON) {
                t = p->duration - remain;
                t /= p->duration / 2;
                if(t < 1.0f) {
                        t = -0.5f * ((float)sqrt(1 - t * t) - 1);
                } else {
                        t = t - 2;
                        t = 0.5f * ((float)sqrt(t - t * t) + 1);
                }
        } else {
                t = 1.0f;
        }
        p->remain       = remain;
        __action_update_target(p, t);
}

static void __action_ease_gravity(struct naction *p, float delta)
{
        if(p->destinations->len == 0) {
                p->finish = 1;
                goto end;
        }
        union vec4 destination          = array_get(p->destinations, union vec4, p->index);
        union vec4 s                    = *p->target;

        if(smemcmp(&destination, &s, sizeof(union vec4)) == 0) goto finish;

        union vec4 direction            = vec4_normalize(vec4_sub(destination, s));

        s                               = vec4_add(s, vec4_mul_scalar(direction, p->velocity * delta));
        s                               = vec4_add(s, vec4_mul_scalar(direction, p->accelerate * delta * delta * 0.5f));

        *p->target                      = s;
        p->velocity                     = p->velocity + p->accelerate * delta;

        if(p->velocity > p->max_velocity) {
                p->velocity = p->max_velocity;
        }

        if(smemcmp(&destination, &s, sizeof(union vec4)) == 0) goto finish;

        union vec4 new_direction        = vec4_normalize(vec4_sub(destination, s));

        float c                         = vec4_length(vec4_add(direction, new_direction));

        if(c > 1) goto end;

finish:;
        p->index++;
        if(p->index == p->destinations->len) {
                *p->target              = destination;
                p->finish               = 1;
        }
end:;
}

static void __action_ease_force(struct naction *p)
{
        *p->target = p->destination;
        p->finish = 1;
}

static void __action_ease_delay(struct naction *p, float delta)
{
        p->delay -= delta;
        if(p->delay <= 0) {
                p->finish = 1;
        }
}

u8 naction_update(struct naction *p, float delta)
{
        u8 finish = 1;
        u8 last_finish = p->finish;
        if( ! p->finish) {
                switch (p->ease_type) {
                        case NATIVE_UI_EASE_LINEAR:
                                __action_ease_linear(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUADRATIC_IN:
                                __action_ease_quadratic_in(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUADRATIC_OUT:
                                __action_ease_quadratic_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUADRATIC_IN_OUT:
                                __action_ease_quadratic_in_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_CUBIC_IN:
                                __action_ease_cubic_in(p, delta);
                                break;

                        case NATIVE_UI_EASE_CUBIC_OUT:
                                __action_ease_cubic_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_CUBIC_IN_OUT:
                                __action_ease_cubic_in_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUARTIC_IN:
                                __action_ease_quartic_in(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUARTIC_OUT:
                                __action_ease_quartic_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUARTIC_IN_OUT:
                                __action_ease_quartic_in_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUINTIC_IN:
                                __action_ease_quintic_in(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUINTIC_OUT:
                                __action_ease_quintic_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_QUINTIC_IN_OUT:
                                __action_ease_quintic_in_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_SINUSOIDAL_IN:
                                __action_ease_sinusoidal_in(p, delta);
                                break;

                        case NATIVE_UI_EASE_SINUSOIDAL_OUT:
                                __action_ease_sinusoidal_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_SINUSOIDAL_IN_OUT:
                                __action_ease_sinusoidal_in_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_EXPONENTIAL_IN:
                                __action_ease_exponential_in(p, delta);
                                break;

                        case NATIVE_UI_EASE_EXPONENTIAL_OUT:
                                __action_ease_exponential_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_EXPONENTIAL_IN_OUT:
                                __action_ease_exponential_in_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_CIRCULAR_IN:
                                __action_ease_circular_in(p, delta);
                                break;

                        case NATIVE_UI_EASE_CIRCULAR_OUT:
                                __action_ease_circular_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_CIRCULAR_IN_OUT:
                                __action_ease_circular_in_out(p, delta);
                                break;

                        case NATIVE_UI_EASE_GRAVITY:
                                __action_ease_gravity(p, delta);
                                break;

                        case NATIVE_UI_EASE_SEQUENCE:
                                {
                                        struct list_head *head;
                                        p->finish = 1;
                                        list_for_each(head, &p->children) {
                                                struct naction *child = (struct naction *)
                                                        ((char *)head - offsetof(struct naction, head));
                                                if(child->finish) {
                                                        continue;
                                                } else {
                                                        u8 child_finish = naction_update(child, delta);
                                                        if(child_finish && head == p->children.prev) {
                                                                p->finish = 1;
                                                        } else {
                                                                p->finish = 0;
                                                        }
                                                        break;
                                                }
                                        }
                                }
                                break;

                        case NATIVE_UI_EASE_PARALLEL:
                                {
                                        struct list_head *head;
                                        p->finish = 1;
                                        list_for_each(head, &p->children) {
                                                struct naction *child = (struct naction *)
                                                        ((char *)head - offsetof(struct naction, head));
                                                u8 child_finish = naction_update(child, delta);
                                                p->finish &= child_finish;
                                        }
                                }
                                break;

                        case NATIVE_UI_EASE_FORCE:
                                __action_ease_force(p);
                                break;
                        case NATIVE_UI_EASE_DELAY:
                                __action_ease_delay(p, delta);
                                break;

                        default:
                                p->finish = 1;
                                break;
                }
        }
        finish &= p->finish;
        if(!last_finish && p->callback) {
                p->callback(p->callback_data);
        }
        if(finish) {
                if(p->repeat == -1) {
                        __action_reset(p);
                        finish = 0;
                } else if(p->repeat > 0) {
                        p->repeat--;
                        __action_reset(p);
                        finish = 0;
                }
        }
        return finish;
}

struct naction *naction_sequence(struct naction *p, ...)
{
        struct naction *se = __action_alloc_sequence();
        va_list parg;
        va_start(parg, p);
        struct naction *child    = p;
        while(child) {
                list_add_tail(&child->head, &se->children);
                child = va_arg(parg, struct naction *);
        }
        va_end(parg);
        return se;
}

struct naction *naction_repeat(int count, struct naction *p, ...)
{
        struct naction *se = __action_alloc_repeat(count);
        va_list parg;
        va_start(parg, p);
        struct naction *child    = p;
        while(child) {
                list_add_tail(&child->head, &se->children);
                child = va_arg(parg, struct naction *);
        }
        va_end(parg);
        return se;
}

struct naction *naction_parallel(struct naction *p, ...)
{
        struct naction *se = __action_alloc_parallel();
        va_list parg;
        va_start(parg, p);
        struct naction *child    = p;
        while(child) {
                list_add_tail(&child->head, &se->children);
                child = va_arg(parg, struct naction *);
        }
        va_end(parg);
        return se;
}

/*
 * action key manipulations
 */
void naction_key_init(struct naction_key *p)
{
        INIT_LIST_HEAD(&p->key_head);
        INIT_LIST_HEAD(&p->actions);
}

void naction_key_clear(struct naction_key *key)
{
        struct list_head *action_head;
        list_while_not_singular(action_head, &key->actions) {
                struct naction *a = (struct naction *)
                        ((char *)action_head - offsetof(struct naction, head));
                naction_free(a);
        }
        list_del_init(&key->key_head);
}

void naction_key_add_action(struct naction_key *p, struct naction *a)
{
        list_del(&a->head);
        list_add_tail(&a->head, &p->actions);
}

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
#ifndef __NATIVE_UI_UTIL_H__
#define __NATIVE_UI_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <UIKit/UIKit.h>
#include <native_ui/types.h>

UIView *custom_his_test(UIView *current, CGPoint point, UIEvent *event);

void util_touch_began(struct native_view *p, union vec2 liv);

void util_touch_moved(struct native_view *p, union vec2 liv);

void util_touch_ended(struct native_view *p, union vec2 liv);

void util_touch_cancelled(struct native_view *p, union vec2 liv);

#ifdef __cplusplus
}
#endif

#endif

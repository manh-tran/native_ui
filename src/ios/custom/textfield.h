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
#ifndef __NATIVE_UI_IOS_CUSTOM_TEXTFIELD_H__
#define __NATIVE_UI_IOS_CUSTOM_TEXTFIELD_H__

#ifdef __cplusplus
extern "C" {
#endif

#import <UIKit/UIKit.h>

@interface CustomTextField : UITextField<UITextFieldDelegate>

@property struct native_view *native_ptr;

@end

#ifdef __cplusplus
}
#endif

#endif

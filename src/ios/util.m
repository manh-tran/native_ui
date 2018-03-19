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
#import <native_ui/util.h>
#import <native_ui/view.h>

static u8 __view_touched = 0;

UIView *custom_his_test(UIView *current, CGPoint point, UIEvent *event)
{
        if(__view_touched) return nil;

        __block UIView *hitView = current;

        if(current.hidden) {
                return nil;
        }

        if (!current.userInteractionEnabled || current.alpha < 0.01)
        {
            hitView = nil;
        }

        if (![current pointInside:point withEvent:event])
        {
            hitView = nil;
        }


        [current.subviews enumerateObjectsWithOptions:NSEnumerationReverse usingBlock:^(id obj, NSUInteger idx, BOOL *stop) {

            CGPoint thePoint = [current convertPoint:point toView:obj];

            UIView *theSubHitView = [obj hitTest:thePoint withEvent:event];

            if (theSubHitView != nil)
            {
                hitView = theSubHitView;

               *stop = YES;
            }

        }];

        return hitView;
}

void util_touch_began(struct native_view *p, union vec2 liv)
{
        __view_touched = 1;
        native_view_touch_began(p, liv);
}

void util_touch_moved(struct native_view *p, union vec2 liv)
{
        native_view_touch_moved(p, liv);
}

void util_touch_ended(struct native_view *p, union vec2 liv)
{
        native_view_touch_ended(p, liv);
        __view_touched = 0;
}

void util_touch_cancelled(struct native_view *p, union vec2 liv)
{
        native_view_touch_cancelled(p, liv);
        __view_touched = 0;
}

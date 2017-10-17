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
#import "view.h"
#import <native_ui/util.h>
#import <native_ui/view.h>

@interface CustomView ()

@end

@implementation CustomView {
        CGPoint screen_began_point;
        CGPoint view_began_point;
}

-(void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
        UITouch *touch          = [touches anyObject];
        view_began_point        = [touch locationInView:self];
        screen_began_point      = [touch locationInView:nil];

        util_touch_began(self.native_ptr, (union vec2){view_began_point.x, view_began_point.y});
}

-(void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
        UITouch *touch          = [touches anyObject];
        CGPoint sbp             = [touch locationInView:nil];

        util_touch_moved(self.native_ptr, (union vec2){
                view_began_point.x + (sbp.x - screen_began_point.x),
                view_began_point.y + (sbp.y - screen_began_point.y)
        });
}

-(void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
        UITouch *touch          = [touches anyObject];
        CGPoint location        = [touch locationInView:self];
        CGPoint sbp             = [touch locationInView:nil];

        if([self pointInside:location withEvent:event]) {
                util_touch_ended(self.native_ptr, (union vec2){
                        view_began_point.x + (sbp.x - screen_began_point.x),
                        view_began_point.y + (sbp.y - screen_began_point.y)
                });
        } else {
                util_touch_cancelled(self.native_ptr, (union vec2){
                        view_began_point.x + (sbp.x - screen_began_point.x),
                        view_began_point.y + (sbp.y - screen_began_point.y)
                });
        }
}

-(void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
        UITouch *touch          = [touches anyObject];
        CGPoint sbp             = [touch locationInView:nil];

        util_touch_cancelled(self.native_ptr, (union vec2){
                view_began_point.x + (sbp.x - screen_began_point.x),
                view_began_point.y + (sbp.y - screen_began_point.y)
        });
}

- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event
{
   return custom_his_test(self, point, event);
}


@end

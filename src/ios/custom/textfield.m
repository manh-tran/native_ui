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
#import "textfield.h"
#import <native_ui/util.h>
#import <native_ui/view.h>
#import <cherry/math/math.h>
#import <cherry/stdio.h>

@interface CustomTextField ()

@end

@implementation CustomTextField {
        int open;
        CGPoint screen_began_point;
        CGPoint view_began_point;
}

-(instancetype)init
{
        id instance = [super init];

        self.delegate = self;

        open = 0;

        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];

        return instance;
}

- (void)keyboardWillShow:(NSNotification *)notification
{
        if(!open) return;

        CGSize keyboardSize = [[[notification userInfo] objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue].size;

        CGRect abFrame = [self convertRect:self.bounds toView:nil];

        float y  = __root->size.height - keyboardSize.height;
        float vy = abFrame.origin.y + abFrame.size.height;

        if(vy > y) {
                [UIView animateWithDuration:0.3 animations:^{
                        native_view_set_position(__root, (union vec2){__root->size.width/2, __root->size.height/2 - (vy - y)});
                }];
        }
}

-(void)keyboardWillHide:(NSNotification *)notification
{
        if(!open) return;

        [UIView animateWithDuration:0.3 animations:^{
                native_view_set_position(__root, (union vec2){__root->size.width/2, __root->size.height/2});

        }];
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

-(CGRect)editingRectForBounds:(CGRect)bounds{
    return CGRectOffset([self textRectForBounds:bounds], 0, -0.5);
}
-(CGRect)placeholderRectForBounds:(CGRect)bounds{
    return CGRectOffset([self editingRectForBounds:bounds], 0, 0);
}

- (CGRect)textRectForBounds:(CGRect)bounds {
        CGRect val = [super textRectForBounds:bounds];
        val.origin.x += 8;
        val.size.width -= 8;
        return val;
}

-(void)textFieldDidBeginEditing:(UITextField *)textField {
        open = 1;
}

-(void)textFieldDidEndEditing:(UITextField *)textField {
        open = 0;
}

// It is important for you to hide the keyboard
- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}

@end

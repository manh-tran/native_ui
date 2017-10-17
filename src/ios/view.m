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
#import <cherry/platform.h>

#if OS == IOS
#import <native_ui/view.h>
#import <native_ui/view_layout_controller.h>
#import <native_ui/align.h>
#import <native_ui/parser.h>

#import <cherry/memory.h>
#import <cherry/list.h>
#import <cherry/string.h>
#import <cherry/stdio.h>
#import <cherry/array.h>
#import <cherry/map.h>
#import <cherry/math/math.h>
#import <UIKit/UIKit.h>

#import "custom/imageview.h"
#import "custom/view.h"
#import "custom/textfield.h"
#import "custom/textview.h"
#import "custom/label.h"

#import <QuartzCore/QuartzCore.h>

pthread_mutex_t         __shared_nview_touch_lock;

struct nview *nview_alloc()
{
        struct nview *p           = smalloc(sizeof(struct nview), nview_free);
        nview_init(p);

        p->ptr                          = NULL;
        nview_show_view(p);
        return p;
}

void nview_free(struct nview *p)
{
        nview_free_common(p);
        /*
         * deallocate view content
         */
        if(p->ptr) {
                UIView *view            = (__bridge id)(p->ptr);
                [view removeFromSuperview];
                CFRelease(p->ptr);
                CFRelease(p->ptr);
        }
        sfree(p);
}

/*
 * show views
 */
static void __change_view(struct nview *p, UIView *new_view)
{
        if(p->ptr) {
                UIView *view            = (__bridge id)(p->ptr);
                [view removeFromSuperview];
                CFRelease(p->ptr);
                CFRelease(p->ptr);
                p->ptr                  = NULL;
        }
        p->ptr                          = CFBridgingRetain(new_view);

        if(p->parent && p->parent->ptr) {
                UIView *parent          = (__bridge id)(p->parent->ptr);
                [parent addSubview:new_view];
        }
        new_view.layer.allowsEdgeAntialiasing = YES;
        [new_view setMultipleTouchEnabled:NO];
}

void nview_show_view(struct nview *p)
{
        p->type                 = NATIVE_UI_VIEW;
        CustomView *view        = [[CustomView alloc] init];
        view.native_ptr        = p;
        __change_view(p, view);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void nview_show_label(struct nview *p)
{
        p->type                 = NATIVE_UI_LABEL;
        CustomLabel *view       = [[CustomLabel alloc] init];
        view.native_ptr         = p;
        __change_view(p, view);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);

        nview_on_change_label(p);
}

void nview_show_image(struct nview *p, char *path)
{
        p->type                 = NATIVE_UI_IMAGE;
        CustomImageView *view   = [[CustomImageView alloc] init];
        view.native_ptr         = p;
        __change_view(p, view);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);

        nview_on_change_imageview(p, path);
}

void nview_show_textfield(struct nview *p)
{
        p->type                 = NATIVE_UI_TEXTFIELD;

        CustomTextField* text = [[CustomTextField alloc] init];
        text.native_ptr        = p;
        [text setPlaceholder:@"Input"];
        text.keyboardType = UIKeyboardTypeDefault;
        text.borderStyle = UITextBorderStyleNone;
        text.allowsEditingTextAttributes = TRUE;
        __change_view(p, text);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void nview_show_textview(struct nview *p)
{
        p->type                 = NATIVE_UI_TEXTVIEW;

        CustomTextView* text = [[CustomTextView alloc] init];
        text.native_ptr        = p;
        text.keyboardType = UIKeyboardTypeDefault;
        text.allowsEditingTextAttributes = TRUE;
        __change_view(p, text);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);
}

void nview_show_listview(struct nview *p)
{
        p->type                 = NATIVE_UI_LISTVIEW;
        CustomView *view        = [[CustomView alloc] init];
        view.native_ptr        = p;
        __change_view(p, view);

        nview_set_position(p, p->position);
        nview_set_size(p, p->size);
        nview_set_scale(p, p->scale);
        nview_set_rotation(p, p->rotation);
        nview_set_clip(p, p->clip);
        nview_set_color(p, p->color, p->border_color);
        nview_set_alpha(p, p->alpha);
        nview_set_anchor(p, p->anchor);
        nview_set_visible(p, p->visible);
        nview_set_user_interaction_enabled(p, p->user_interaction_enabled);

        nview_on_change_listview(p);
}

/*
 * parent child
 */
void nview_add_child(struct nview *p, struct nview *c)
{
        if(c->parent && c->parent->name_to_child) {
                map_remove_key(c->parent->name_to_child, qskey(c->name));
        }

        list_del(&c->head);
        list_add_tail(&c->head, &p->children);
        c->parent = p;

        if(c->name->len) {
                if(!p->name_to_child) {
                        p->name_to_child = map_alloc(sizeof(struct nview *));
                        map_set(p->name_to_child, qskey(c->name), &c);
                }
        }

        UIView *parent          = (__bridge id)(p->ptr);
        UIView *child           = (__bridge id)(c->ptr);

        [child removeFromSuperview];
        [parent addSubview:child];
}

void nview_remove_from_parent(struct nview *p)
{
        if(!p->parent) return;

        if(p->parent && p->parent->name_to_child) {
                map_remove_key(p->parent->name_to_child, qskey(p->name));
        }

        list_del_init(&p->head);
        p->parent = NULL;

        UIView *child           = (__bridge id)(p->ptr);

        [child removeFromSuperview];
}

/*
 * set transform
 */
static void __reset_transform(struct nview *p)
{
        UIView *view                    = (__bridge id)(p->ptr);



        if(p->rotation.x != 0 || p->rotation.y != 0) {

                CATransform3D mat = CATransform3DIdentity;
                if(p->rotation.x != 0 || p->rotation.y != 0
                        || p->rotation.z != 0 || p->scale.x != 1 || p->scale.y != 1
                        ) {
                        mat.m34 = -1.0f / 500;

                        if(p->rotation.x != 0) mat = CATransform3DRotate(mat, DEG_TO_RAD(p->rotation.x), 1.0f, 0.0f, 0.0f);
                        if(p->rotation.y != 0) mat = CATransform3DRotate(mat, DEG_TO_RAD(p->rotation.y), 0.0f, 1.0f, 0.0f);
                        if(p->rotation.x != 0) mat = CATransform3DRotate(mat, DEG_TO_RAD(p->rotation.z), 0.0f, 0.0f, 1.0f);
                        mat = CATransform3DScale(mat, p->scale.x, p->scale.y, 1.0f);
                }

                view.layer.transform = mat;
        } else {
                CGAffineTransform mat = CGAffineTransformIdentity;
                mat = CGAffineTransformRotate(mat, DEG_TO_RAD(p->rotation.z));
                mat = CGAffineTransformScale(mat, p->scale.x, p->scale.y);

                view.transform = mat;
        }



}

void nview_set_position(struct nview *p, union vec2 position)
{
        p->position     = position;

        UIView *view    = (__bridge id)(p->ptr);

        [view setCenter:CGPointMake(
                position.x,
                position.y
        )];

}

void nview_set_size(struct nview *p, union vec2 size)
{
        p->size         = size;
        UIView *view    = (__bridge id)(p->ptr);

        // CGRect newFrame = CGRectMake(p->position.x - size.width/2, p->position.y - size.height/2, size.width, size.height);
        CGRect newFrame = CGRectMake(0,0, size.width, size.height);
        // view.frame      = newFrame;
        view.bounds     = newFrame;
}

void nview_set_scale(struct nview *p, union vec2 scale)
{
        p->scale                        = scale;

        __reset_transform(p);
}

void nview_set_rotation(struct nview *p, union vec3 rotation)
{
        p->rotation                     = rotation;

        __reset_transform(p);
}

void nview_set_anchor(struct nview *p, union vec2 anchor)
{
        p->anchor                       = anchor;

        UIView *view                    = (__bridge id)(p->ptr);

        view.layer.anchorPoint          = CGPointMake(anchor.x, anchor.y);
}

void nview_set_color(struct nview *p, union vec4 *color, union vec4 *border)
{
        if(color) {
                if(!p->color) {
                        p->color        = smalloc(sizeof(union vec4), sfree);
                }
                (*p->color)             = (*color);
        } else {
                if(p->color) {
                        sfree(p->color);
                        p->color        = NULL;
                }
        }

        if(border) {
                if(!p->border_color) {
                        p->border_color = smalloc(sizeof(union vec4), sfree);
                }
                (*p->border_color)      = (*border);
        } else {
                if(p->border_color) {
                        sfree(p->border_color);
                        p->border_color = NULL;
                }
        }

        UIView *view                    = (__bridge id)(p->ptr);

        if(p->color && p->border_color) {
                [view setBackgroundColor:[UIColor colorWithRed:color->r green:color->g blue:color->b alpha:color->a]];
                view.layer.borderColor = [[UIColor colorWithRed:border->r green:border->g blue:border->b alpha:border->a] CGColor];
                view.layer.borderWidth = 0.6f;
        } else if(p->color) {
                [view setBackgroundColor:[UIColor colorWithRed:color->r green:color->g blue:color->b alpha:color->a]];
                view.layer.borderColor = nil;
                view.layer.borderWidth = 0;
        } else if(p->border_color) {
                view.layer.borderColor = [UIColor colorWithRed:border->r green:border->g blue:border->b alpha:border->a];
                view.layer.borderWidth = 0.6f;

                if(p->type == NATIVE_UI_LABEL) {
                        [view setBackgroundColor:[UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
                } else {
                        [view setBackgroundColor:nil];
                }
        } else {
                if(p->type == NATIVE_UI_LABEL) {
                        [view setBackgroundColor:[UIColor colorWithRed:0 green:0 blue:0 alpha:0]];
                } else {
                        [view setBackgroundColor:nil];
                }
                view.layer.borderColor = nil;
                view.layer.borderWidth = 0;
        }
}

void nview_set_visible(struct nview *p, u8 visible)
{
        p->visible = visible;

        UIView *view                    = (__bridge id)(p->ptr);

        [view setHidden:visible == 1 ? NO : YES];
}

void nview_set_alpha(struct nview *p, float alpha)
{
        p->alpha                        = alpha;

        UIView *view                    = (__bridge id)(p->ptr);
        [view setAlpha:alpha];
}

void nview_set_clip(struct nview *p, u8 clip)
{
        p->clip                         = clip;
        UIView *view                    = (__bridge id)(p->ptr);
        [view setClipsToBounds: clip == 1 ? YES : NO];

        if(p->clip_rounds.x > 0) {
                view.layer.cornerRadius         = p->clip_rounds.x;
                view.layer.masksToBounds        = clip == 1 ? YES : NO;
        } else {
                view.layer.cornerRadius         = 0;
                view.layer.masksToBounds        = NO;
                [view setClipsToBounds: clip == 1 ? YES : NO];
        }
}

/*
 * touches
 */
void nview_set_user_interaction_enabled(struct nview *p, u8 enabled)
{
        p->user_interaction_enabled     = enabled;
        UIView *view                    = (__bridge id)(p->ptr);
        view.userInteractionEnabled     = enabled;
}

union vec2 nview_convert_point_to_view(struct nview *from, union vec2 point, struct nview *to)
{
        UIView *from_view       = (__bridge id)(from->ptr);
        UIView *to_view         = (__bridge id)(to->ptr);
        CGPoint ret = [from_view convertPoint:CGPointMake(point.x, point.y) toView:to_view];
        return (union vec2){ret.x, ret.y};
}

#endif

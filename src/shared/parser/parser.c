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
#include <native_ui/parser.h>
#include <native_ui/view.h>
#include <native_ui/view_layout_controller.h>
#include <native_ui/align.h>
#include <native_ui/touch_handle.h>
#include <native_ui/view_controller.h>
#include <native_ui/file.h>
#include <cherry/memory.h>
#include <cherry/list.h>
#include <cherry/string.h>
#include <cherry/array.h>
#include <cherry/map.h>
#include <cherry/math/math.h>
#include <cherry/xml/xml.h>
#include <cherry/stdio.h>
#include "delegate/delegate.h"
#include "common_delegate/delegate.h"
#include "end_delegate/delegate.h"

typedef struct native_view *(*view_parse_delegate)(struct xml_element *, struct native_parser *, struct native_parser *);
typedef void(*view_parse_common_delegate)(struct native_view *, struct xml_attribute *, struct native_parser *, struct native_parser *);

static struct map *__parse_function                     = NULL;
static struct map *__parse_end_function                 = NULL;
static struct map *__parse_common_function              = NULL;
static struct map *__xml_cache                          = NULL;

static void __clear()
{
        if(__parse_function) {
                map_free(__parse_function);
                map_free(__parse_common_function);
                map_free(__parse_end_function);
                map_deep_free(__xml_cache, struct xml_element *, xml_free);
                __parse_function                        = NULL;
                __parse_common_function                 = NULL;
                __parse_end_function                    = NULL;
                __xml_cache                             = NULL;
        }
}

static void __init()
{
        if(!__parse_function) {
                cache_add(__clear);
                __parse_function                = map_alloc(sizeof(view_parse_delegate));
                __parse_end_function            = map_alloc(sizeof(view_parse_delegate));
                __parse_common_function         = map_alloc(sizeof(view_parse_common_delegate));
                __xml_cache                     = map_alloc(sizeof(struct xml_element *));

                /*
                 * register view parse function
                 */
                map_set(__parse_function, qlkey("view"), &(view_parse_delegate){parser_view});
                map_set(__parse_function, qlkey("label"), &(view_parse_delegate){parser_label});
                map_set(__parse_function, qlkey("textfield"), &(view_parse_delegate){parser_textfield});
                map_set(__parse_function, qlkey("textview"), &(view_parse_delegate){parser_textview});
                map_set(__parse_function, qlkey("listview"), &(view_parse_delegate){parser_listview});
                map_set(__parse_function, qlkey("pageview"), &(view_parse_delegate){parser_pageview});
                map_set(__parse_function, qlkey("image"), &(view_parse_delegate){parser_image});
                map_set(__parse_function, qlkey("link"), &(view_parse_delegate){parser_link});
                map_set(__parse_function, qlkey("touch"), &(view_parse_delegate){parser_touch});
                map_set(__parse_function, qlkey("controller"), &(view_parse_delegate){parser_controller});
                map_set(__parse_function, qlkey("template"), &(view_parse_delegate){parser_template});
                /*
                 * register view parse end function
                 */
                map_set(__parse_end_function, qlkey("template"), &(view_parse_delegate){parser_end_template});
                /*
                 * register view attribute parse function
                 */
                map_set(__parse_common_function, qlkey("width"), &(view_parse_common_delegate){parse_common_width});
                map_set(__parse_common_function, qlkey("height"), &(view_parse_common_delegate){parse_common_height});
                map_set(__parse_common_function, qlkey("x"), &(view_parse_common_delegate){parse_common_x});
                map_set(__parse_common_function, qlkey("y"), &(view_parse_common_delegate){parse_common_y});
                map_set(__parse_common_function, qlkey("scale"), &(view_parse_common_delegate){parse_common_scale});
                map_set(__parse_common_function, qlkey("rotation_x"), &(view_parse_common_delegate){parse_common_rotation_x});
                map_set(__parse_common_function, qlkey("rotation_y"), &(view_parse_common_delegate){parse_common_rotation_y});
                map_set(__parse_common_function, qlkey("rotation_z"), &(view_parse_common_delegate){parse_common_rotation_z});
                map_set(__parse_common_function, qlkey("type"), &(view_parse_common_delegate){parse_common_type});
                map_set(__parse_common_function, qlkey("align"), &(view_parse_common_delegate){parse_common_align});
                map_set(__parse_common_function, qlkey("touch"), &(view_parse_common_delegate){parse_common_touch});
                map_set(__parse_common_function, qlkey("color"), &(view_parse_common_delegate){parse_common_color});
                map_set(__parse_common_function, qlkey("border"), &(view_parse_common_delegate){parse_common_border});
                map_set(__parse_common_function, qlkey("alpha"), &(view_parse_common_delegate){parse_common_alpha});
                map_set(__parse_common_function, qlkey("visible"), &(view_parse_common_delegate){parse_common_visible});
                map_set(__parse_common_function, qlkey("clip"), &(view_parse_common_delegate){parse_common_clip});
                map_set(__parse_common_function, qlkey("round"), &(view_parse_common_delegate){parse_common_round});
                map_set(__parse_common_function, qlkey("margin_top"), &(view_parse_common_delegate){parse_common_margin_top});
                map_set(__parse_common_function, qlkey("margin_left"), &(view_parse_common_delegate){parse_common_margin_left});
                map_set(__parse_common_function, qlkey("margin_bottom"), &(view_parse_common_delegate){parse_common_margin_bottom});
                map_set(__parse_common_function, qlkey("margin_right"), &(view_parse_common_delegate){parse_common_margin_right});
                map_set(__parse_common_function, qlkey("padding_top"), &(view_parse_common_delegate){parse_common_padding_top});
                map_set(__parse_common_function, qlkey("padding_left"), &(view_parse_common_delegate){parse_common_padding_left});
                map_set(__parse_common_function, qlkey("padding_bottom"), &(view_parse_common_delegate){parse_common_padding_bottom});
                map_set(__parse_common_function, qlkey("padding_right"), &(view_parse_common_delegate){parse_common_padding_right});
                map_set(__parse_common_function, qlkey("hash"), &(view_parse_common_delegate){parse_common_hash});
                map_set(__parse_common_function, qlkey("touch_delegate"), &(view_parse_common_delegate){parse_common_touch_delegate});
                map_set(__parse_common_function, qlkey("intercept_vertical"), &(view_parse_common_delegate){parse_common_intercept_vertical});
                map_set(__parse_common_function, qlkey("intercept_horizontal"), &(view_parse_common_delegate){parse_common_intercept_horizontal});
                map_set(__parse_common_function, qlkey("anchor_x"), &(view_parse_common_delegate){parse_common_anchor_x});
                map_set(__parse_common_function, qlkey("anchor_y"), &(view_parse_common_delegate){parse_common_anchor_y});
                map_set(__parse_common_function, qlkey("weights"), &(view_parse_common_delegate){parse_common_weights});
                map_set(__parse_common_function, qlkey("weight"), &(view_parse_common_delegate){parse_common_weight});
        }
}

void __native_view_hash_head_free(struct list_head *p)
{
        if(!list_singular(p)) {
                struct native_view *view = (struct native_view *)
                        ((char *)p->next - offsetof(struct native_view, hash_head));
                list_del_init(p);
                native_view_free(view);
        }
        sfree(p);
}

struct native_parser *native_parser_alloc()
{
        __init();
        struct native_parser *p    = smalloc(sizeof(struct native_parser), native_parser_free);
        INIT_LIST_HEAD(&p->view);
        p->hash_views                   = map_alloc(sizeof(struct list_head *));
        p->hash_touches                 = map_alloc(sizeof(struct native_touch *));
        p->hash_templates               = map_alloc(sizeof(struct xml_element *));
        p->controller                   = NULL;
        p->template_scope               = 0;
        p->parent                       = NULL;
        return p;
}

void native_parser_free(struct native_parser *p)
{
        map_deep_free(p->hash_views, struct list_head *, __native_view_hash_head_free);
        map_deep_free(p->hash_touches, struct native_touch *, native_touch_free);
        map_free(p->hash_templates);
        if(!list_singular(&p->view)) {
                struct native_view *view = (struct native_view *)
                        ((char *)p->view.next - offsetof(struct native_view, parser));
                list_del_init(p->view.next);
                native_view_free(view);
        }
        list_del_init(&p->view);
        sfree(p);
}

struct native_view *native_parser_get_hash_view(struct native_parser *p, char *key, size_t len)
{
        if(map_has_key(p->hash_views, key, len)) {
                struct list_head *head = map_get(p->hash_views, struct list_head *, key, len);
                if(!list_singular(head)) {
                        struct native_view *view = (struct native_view *)
                                ((char *)head->next - offsetof(struct native_view, hash_head));
                        return view;
                }
        }
        return NULL;
}

struct native_touch *native_parser_get_touch(struct native_parser *p, char *key, size_t len)
{
        return map_get(p->hash_touches, struct native_touch *, key, len);
}

void native_parser_parse_file(struct native_parser *p, char *file, struct native_parser *parent)
{
        p->parent = parent;
        // int type                = nfile_type(file);
        struct string *xml_key  = string_alloc_chars(file, strlen(file));
        // string_cat_int(xml_key, type);

        if( ! map_has_key(__xml_cache, qskey(xml_key))) {
                // struct xml_element *xml         = xml_parse(file, type);
                struct xml_element *xml = xml_parse(file);
                map_set(__xml_cache, qskey(xml_key), &xml);
        }

        struct xml_element *root                = map_get(__xml_cache, struct xml_element *, qskey(xml_key));
        string_free(xml_key);

        struct native_view *current_view        = NULL;

        struct list_head *head = &root->element_head;
        struct native_view *v = NULL;
        while(head) {
                /*
                 * reset temp view
                 */
                v = NULL;
                /*
                 * stop parsing view when in template scope
                 */

                struct xml_element *e = (struct xml_element *)
                        ((char *)head - offsetof(struct xml_element, element_head));

                if(p->template_scope)
                        goto check_head;
                /*
                 * logic
                 */
                view_parse_delegate *delegate = map_get_pointer(__parse_function, qskey(e->name));
                if(*delegate) {
                        v = (*delegate)(e, p, parent);
                }
                if(v) {
                        if(current_view) {
                                native_view_add_child(current_view, v);
                        } else {
                            current_view = v;
                            if(p->controller) {
                                    native_controller_set_view(p->controller, v);
                            }
                        }
                }
                if(v) {
                        struct list_head *attr_head;
                        list_for_each(attr_head, &e->attributes) {
                                struct xml_attribute *a = (struct xml_attribute *)
                                        ((char *)attr_head - offsetof(struct xml_attribute, head));
                               view_parse_common_delegate *common_delegate = map_get_pointer(__parse_common_function, qskey(a->name));
                               if(*common_delegate) {
                                       (*common_delegate)(v, a, p, parent);
                               }
                        }
                }
                if(v) {
                        native_view_set_anchor(v, v->anchor);
                        native_view_set_position(v, v->position);
                        native_view_set_size(v, (union vec2){v->align->fixed_width, v->align->fixed_height});
                        native_view_set_scale(v, v->scale);
                        native_view_set_rotation(v, v->rotation);
                        native_view_set_clip(v, v->clip);
                        native_view_set_alpha(v, v->alpha);
                        native_view_set_color(v, v->color, v->border_color);
                        native_view_set_visible(v, v->visible);

                }
                /*
                 * invalidate head for next check
                 */
        check_head:;
                head = NULL;
                if(!list_singular(&e->children)) {
                        /*
                         * check first child
                         */
                        head = e->children.next;
                        if(v) {
                            current_view = v;
                        }
                } else {
                        /*
                         * check next sibling
                         * if e is last node than check next sibling of parent recursily
                         */
                        while(e->parent) {
                                if(e->element_head.next != &e->parent->children) {
                                        view_parse_delegate *end_delegate = map_get_pointer(__parse_end_function, qskey(e->name));
                                        if(*end_delegate) {
                                                (*end_delegate)(e, p, parent);
                                        }
                                        /*
                                         * e has next sibling
                                         */
                                        head = e->element_head.next;
                                        break;
                                } else {
                                        view_parse_delegate *delegate = map_get_pointer(__parse_function, qskey(e->name));
                                        view_parse_delegate *end_delegate = map_get_pointer(__parse_end_function, qskey(e->name));

                                        if(*end_delegate) {
                                                (*end_delegate)(e, p, parent);
                                        }
                                        /*
                                         * change current parent
                                         * ignore this action when in template scope
                                         */
                                        if(delegate && current_view && current_view->parent && !p->template_scope) {
                                                current_view = current_view->parent;
                                        }
                                        /*
                                         * e is last node, check for parent sibling
                                         */
                                        e = e->parent;
                                        /*
                                         * stop at root
                                         */
                                        if(e == root) break;
                                }
                        }
                }
        }

        if(current_view) {
                list_add_tail(&current_view->parser, &p->view);
        }
}

void native_parser_parse_template(struct native_parser *p, struct native_parser *parent, char *key, size_t len)
{
        p->parent = parent;
        struct xml_element *root = map_get(parent->hash_templates, struct xml_element *, key, len);
        struct native_view *current_view        = NULL;

        struct list_head *head = &root->element_head;
        struct native_view *v = NULL;
        while(head) {
                /*
                 * reset temp view
                 */
                v = NULL;
                /*
                 * stop parsing view when in template scope
                 */

                struct xml_element *e = (struct xml_element *)
                        ((char *)head - offsetof(struct xml_element, element_head));

                /*
                 * logic
                 */
                view_parse_delegate *delegate = map_get_pointer(__parse_function, qskey(e->name));
                if(*delegate) {
                        v = (*delegate)(e, p, parent);
                }
                if(v) {
                        if(current_view) {
                                native_view_add_child(current_view, v);
                        } else {
                            current_view = v;
                            if(p->controller) {
                                    native_controller_set_view(p->controller, v);
                            }
                            if(p->controller && parent->controller) {
                                    native_controller_link(parent->controller, p->controller);
                            }
                        }
                }
                if(v) {
                        struct list_head *attr_head;
                        list_for_each(attr_head, &e->attributes) {
                                struct xml_attribute *a = (struct xml_attribute *)
                                        ((char *)attr_head - offsetof(struct xml_attribute, head));
                               view_parse_common_delegate *common_delegate = map_get_pointer(__parse_common_function, qskey(a->name));
                               if(*common_delegate) {
                                       (*common_delegate)(v, a, p, parent);
                               }
                        }
                }
                if(v) {
                        native_view_set_position(v, v->position);
                        native_view_set_size(v, (union vec2){v->align->fixed_width, v->align->fixed_height});
                        native_view_set_scale(v, v->scale);
                        native_view_set_rotation(v, v->rotation);
                        native_view_set_clip(v, v->clip);
                        native_view_set_alpha(v, v->alpha);
                        native_view_set_color(v, v->color, v->border_color);
                        native_view_set_anchor(v, v->anchor);
                        native_view_set_visible(v, v->visible);
                }
                /*
                 * invalidate head for next check
                 */
        check_head:;
                head = NULL;
                if(!list_singular(&e->children)) {
                        /*
                         * check first child
                         */
                        head = e->children.next;
                        if(v) {
                            current_view = v;
                        }
                } else {
                        /*
                         * check next sibling
                         * if e is last node than check next sibling of parent recursily
                         */
                        while(e->parent) {
                                if(e->element_head.next != &e->parent->children) {
                                        view_parse_delegate *end_delegate = map_get_pointer(__parse_end_function, qskey(e->name));
                                        if(*end_delegate) {
                                                (*end_delegate)(e, p, parent);
                                        }
                                        /*
                                         * e has next sibling
                                         */
                                        head = e->element_head.next;
                                        break;
                                } else {
                                        view_parse_delegate *delegate = map_get_pointer(__parse_function, qskey(e->name));
                                        view_parse_delegate *end_delegate = map_get_pointer(__parse_end_function, qskey(e->name));

                                        if(*end_delegate) {
                                                (*end_delegate)(e, p, parent);
                                        }
                                        /*
                                         * change current parent
                                         * ignore this action when in template scope
                                         */
                                        if(delegate && current_view && current_view->parent) {
                                                current_view = current_view->parent;
                                        }
                                        /*
                                         * e is last node, check for parent sibling
                                         */
                                        e = e->parent;
                                        /*
                                         * stop at root
                                         */
                                        if(e == root) break;
                                }
                        }
                }
        }

        if(current_view) {
                list_add_tail(&current_view->parser, &p->view);
        }
}

struct native_view *native_parser_get_view(struct native_parser *p)
{
        if( ! list_singular(&p->view)) {
                return (struct native_view *)((char *)p->view.next - offsetof(struct native_view, parser));
        }
        return NULL;
}

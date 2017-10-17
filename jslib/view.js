var touch_capable = 'ontouchstart' in window ||
         window.DocumentTouch && document instanceof window.DocumentTouch ||
         navigator.maxTouchPoints > 0 ||
         window.navigator.msMaxTouchPoints > 0;

var native_type = {
        VIEW            : 0,
        IMAGE           : 1,
        LABEL           : 2,
        TEXTFIELD       : 3,
        TEXTVIEW        : 4,
        LISTVIEW        : 5,
        PAGEVIEW        : 6
}

function view_helper()
{
        this.alloc();
}

view_helper.prototype.alloc = function()
{
        this.ep = {
                x : 0,
                y : 0
        };

        this.wp = {
                x : 0,
                y : 0
        };

        this.tch = 0;

        this.ctg = null;
        this.ovl = null;
}

view_helper.prototype.view_get_offset_window_left = function(v)
{
        var result = 0;
        var o = v;
        while(o != undefined && o != null) {
                result += o.offsetLeft;
                o = o.parentElement;
        }
        return result;
}


view_helper.prototype.view_get_offset_window_top = function(v)
{
        var result = 0;
        var o = v;
        while(o != undefined && o != null) {
                result += o.offsetTop;
                o = o.parentElement;
        }
        return result;
}

view_helper.prototype.find_view = function(v)
{
        if(v.is_native !== undefined && v.can_touch == 1) return v;

        if(v.parentElement !== undefined && v.parentElement != null) return this.find_view(v.parentElement);

        return null;
}

view_helper.prototype.process_down = function(e, x, y)
{
        if(this.ctg == null) {
                this.ctg = this.find_view(e.target);
                if(this.ctg != null) {
                        this.tch = 1;
                        e.stopPropagation();

                        this.ovl = document.createElement('div');
                        this.ovl.style["position"] = "absolute";
                        this.ovl.style["width"] = "100%";
                        this.ovl.style["height"] = "100%";
                        var self = this;
                        if(!touch_capable) {
                                if (window.PointerEvent) {
                                        this.ovl.addEventListener("pointerdown", function(e){
                                                self.pointerdown(e);
                                        }, false);
                                        this.ovl.addEventListener("pointermove", function(e){
                                                self.pointermove(e);
                                        }, false);
                                        this.ovl.addEventListener("pointerup", function(e){
                                                self.pointerup(e);
                                        }, false);
                                        this.ovl.addEventListener("pointerout", function(e){
                                                self.pointercancel(e);
                                        }, false);
                                } else {
                                        this.ovl.addEventListener("mousedown", function(e){
                                                self.mousedown(e);
                                        }, false);
                                        this.ovl.addEventListener("mousemove", function(e){
                                                self.mousemove(e);
                                        }, false);
                                        this.ovl.addEventListener("mouseup", function(e){
                                                self.mouseup(e);
                                        }, false);
                                        this.ovl.addEventListener("mouseout", function(e){
                                                self.mousecancel(e);
                                        }, false);
                                }
                        } else {
                                this.ovl.addEventListener("touchstart", function(e){
                                        self.touchstart(e);
                                }, false);
                                this.ovl.addEventListener("touchmove", function(e){
                                        self.touchmove(e);
                                }, false);
                                this.ovl.addEventListener("touchend", function(e){
                                        self.touchup(e);
                                }, false);
                                this.ovl.addEventListener("touchcancel", function(e){
                                        self.touchcancel(e);
                                }, false);
                        }                        
                        document.getElementById("root").appendChild(this.ovl);
                        this.ep.x = x- this.view_get_offset_window_left(this.ctg);
                        this.ep.y = y - this.view_get_offset_window_top(this.ctg);
                        this.wp.x = x;
                        this.wp.y = y;

                        _js_nview_touch_began(this.ctg.native_ptr, this.ep.x, this.ep.y);
                }
        }
}

view_helper.prototype.process_move = function(e, x, y)
{
        if(this.tch == 1 && this.ctg != null) {
                var v = {};
                v.x = this.ep.x + x - this.wp.x;
                v.y = this.ep.y + y - this.wp.y;

                _js_nview_touch_moved(this.ctg.native_ptr, v.x, v.y);

                e.stopPropagation();
        }
}

view_helper.prototype.process_up = function(e, x, y)
{
        if(this.tch == 1 && this.ctg != null) {
                var v = {};
                v.x = this.ep.x + x - this.wp.x;
                v.y = this.ep.y + y - this.wp.y;

                _js_nview_touch_ended(this.ctg.native_ptr, v.x, v.y);

                if(this.ctg.native_type == native_type.TEXTFIELD) {
                        this.ctg.custom_content.focus();
                }

                this.ctg = null;
                this.tch = 0;
                this.ovl.remove();
                this.ovl = null;
                e.stopPropagation();
        }
}

view_helper.prototype.process_cancel = function(e, x, y)
{
        if(this.tch == 1 && this.ctg != null
                /*
                 * browser on mobile and desktop behaves very different
                 */
                && ((e.target == this.ovl && !touch_capable) || touch_capable)) {
                var v = {};
                v.x = this.ep.x + x - this.wp.x;
                v.y = this.ep.y + y - this.wp.y;

                _js_nview_touch_cancelled(this.ctg.native_ptr, v.x, v.y);
                this.ctg = null;
                this.tch = 0;
                this.ovl.remove();
                this.ovl = null;
                e.stopPropagation();
        }
}

view_helper.prototype.touchstart = function(e)
{
        e.preventDefault();
        var touches = e.changedTouches;
        for (var i = 0; i < touches.length; i++) {
                this.process_down(e, touches[i].pageX, touches[i].pageY);
        }
}

view_helper.prototype.touchmove = function(e)
{
        e.preventDefault();
        var touches = e.changedTouches;
        for (var i = 0; i < touches.length; i++) {
                this.process_move(e, touches[i].pageX, touches[i].pageY);
        }
}

view_helper.prototype.touchup = function(e)
{
        e.preventDefault();
        var touches = e.changedTouches;
        for (var i = 0; i < touches.length; i++) {
                this.process_up(e, touches[i].pageX, touches[i].pageY);
        }
}

view_helper.prototype.touchcancel = function(e)
{
        e.preventDefault();
        var touches = e.changedTouches;
        for (var i = 0; i < touches.length; i++) {
                this.process_cancel(e, touches[i].pageX, touches[i].pageY);
        }
}

view_helper.prototype.pointerdown = function(e)
{
        e.preventDefault();
        this.process_down(e, e.pageX, e.pageY);
}

view_helper.prototype.pointermove = function(e)
{
        e.preventDefault();
        this.process_move(e, e.pageX, e.pageY);
}

view_helper.prototype.pointerup = function(e)
{
        e.preventDefault();
        this.process_up(e, e.pageX, e.pageY);
}


view_helper.prototype.pointercancel = function(e)
{
        e.preventDefault();
        this.process_cancel(e, e.pageX, e.pageY);
}

view_helper.prototype.mousedown = function(e)
{
        this.process_down(e, e.pageX, e.pageY);
}

view_helper.prototype.mousemove = function(e)
{
        this.process_move(e, e.pageX, e.pageY);
}

view_helper.prototype.mouseup = function(e)
{
        this.process_up(e, e.pageX, e.pageY);
}


view_helper.prototype.mousecancel = function(e)
{
        this.process_cancel(e, e.pageX, e.pageY);
}

/*
 * view base definitions
 */
view_helper.prototype.base_alloc = function()
{
        var view = document.createElement('div');
        view.is_native = 1;
        view.style["position"] = "absolute";
        view.style["width"]     = "0px";
        view.style["height"]     = "0px";

        view.style["perspective"] = "1000px";
        view.style["-webkit-perspective"] = "1000px";

        view.native_anchor = {
                x : 0.5,
                y : 0.5
        };
        view.style["transform-origin"] = "50% 50%";
        view.native_pos = {
                x : 0,
                y : 0
        };
        view.native_rotation = {
                x : 0,
                y : 0,
                z : 0
        };
        view.native_scale = {
                x : 1,
                y : 1
        };
        view.native_size = {
                width : 0,
                height : 0
        };
        view.native_ptr = 0;
        view.can_touch  = 0;
        view.native_visible = 1;
        view.native_alpha = 1;

        var self = this;
        if(!touch_capable) {
                if (window.PointerEvent) {
                        view.addEventListener("pointerdown", function(e){
                                self.pointerdown(e);
                        }, false);
                        view.addEventListener("pointermove", function(e){
                                self.pointermove(e);
                        }, false);
                        view.addEventListener("pointerup", function(e){
                                self.pointerup(e);
                        }, false);
                        view.addEventListener("pointerout", function(e){
                                self.pointercancel(e);
                        }, false);
                } else {
                        view.addEventListener("mousedown", function(e){
                                self.mousedown(e);
                        }, false);
                        view.addEventListener("mousemove", function(e){
                                self.mousemove(e);
                        }, false);
                        view.addEventListener("mouseup", function(e){
                                self.mouseup(e);
                        }, false);
                        view.addEventListener("mouseout", function(e){
                                self.mousecancel(e);
                        }, false);
                }
        } else {
                view.addEventListener("touchstart", function(e){
                        self.touchstart(e);
                }, false);
                view.addEventListener("touchmove", function(e){
                        self.touchmove(e);
                }, false);
                view.addEventListener("touchend", function(e){
                        self.touchup(e);
                }, false);
                view.addEventListener("touchcancel", function(e){
                        self.touchcancel(e);
                }, false);
        }

        view.free = function() {
                self.view_free(view);
        };
        __register_shared_object(view);

        return view;
}

view_helper.prototype.view_set_native_ptr = function(v, p)
{
        v.native_ptr = p;
}

view_helper.prototype.view_alloc = function()
{
        var v = this.base_alloc();
        v.native_type = native_type.VIEW;
        return v;
}

view_helper.prototype.image_alloc = function()
{
        var v = this.base_alloc();
        v.native_type           = native_type.IMAGE;
        v.custom_content        = document.createElement('img');
        v.custom_content.style["position"] = "absolute";
        v.custom_content.style["width"] = "100%";
        v.custom_content.style["height"] = "100%";
        v.custom_content.style["overflow"] = "hidden";
        v.appendChild(v.custom_content);
        this.view_set_clip(v, 1);

        var self = this;
        var view = v.custom_content;
        if(!touch_capable) {
                if (window.PointerEvent) {
                        view.addEventListener("pointerdown", function(e){
                                self.mousedown(e);
                        }, false);
                        view.addEventListener("pointermove", function(e){
                                self.mousemove(e);
                        }, false);
                        view.addEventListener("pointerup", function(e){
                                self.mouseup(e);
                        }, false);
                        view.addEventListener("pointerout", function(e){
                                self.mousecancel(e);
                        }, false);
                } else {
                        view.addEventListener("mousedown", function(e){
                                self.mousedown(e);
                        }, false);
                        view.addEventListener("mousemove", function(e){
                                self.mousemove(e);
                        }, false);
                        view.addEventListener("mouseup", function(e){
                                self.mouseup(e);
                        }, false);
                        view.addEventListener("mouseout", function(e){
                                self.mousecancel(e);
                        }, false);
                }

        } else {
                view.addEventListener("touchstart", function(e){
                        self.touchstart(e);
                }, false);
                view.addEventListener("touchmove", function(e){
                        self.touchmove(e);
                }, false);
                view.addEventListener("touchend", function(e){
                        self.touchup(e);
                }, false);
                view.addEventListener("touchcancel", function(e){
                        self.touchcancel(e);
                }, false);
        }

        return v;
}

view_helper.prototype.label_alloc = function()
{
        var v = this.base_alloc();
        v.native_type = native_type.LABEL;
        v.innerHTML = "";
        v.style["font-size"] = "12px";
        return v;
}

view_helper.prototype.textfield_alloc = function()
{
        var v = this.base_alloc();
        v.native_type = native_type.TEXTFIELD;
        v.custom_content = document.createElement('input');
        v.custom_content.style["position"] = "absolute";
        v.custom_content.style["width"] = "100%";
        v.custom_content.style["height"] = "100%";
        v.custom_content.style["font-size"] = "20px";
        v.custom_content.type = "text";
        v.appendChild(v.custom_content);

        /*
         * if v's can_touch is 1 then stop propagating event to super parent
         * to allow copy/paste/select on textfield
         */
        var self = this;
        var view = v.custom_content;
        if(!touch_capable) {
                if (window.PointerEvent) {
                        view.addEventListener("pointerdown", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mousedown(e);
                                }
                        }, false);
                        view.addEventListener("pointermove", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mousemove(e);
                                }
                        }, false);
                        view.addEventListener("pointerup", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mouseup(e);
                                }
                        }, false);
                        view.addEventListener("pointerout", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mousecancel(e);
                                }
                        }, false);
                } else {
                        view.addEventListener("mousedown", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mousedown(e);
                                }
                        }, false);
                        view.addEventListener("mousemove", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mousemove(e);
                                }
                        }, false);
                        view.addEventListener("mouseup", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mouseup(e);
                                }
                        }, false);
                        view.addEventListener("mouseout", function(e){
                                if(v.can_touch == 1) {
                                        e.stopPropagation();
                                } else {
                                        self.mousecancel(e);
                                }
                        }, false);
                }

        } else {
                view.addEventListener("touchstart", function(e){
                        if(v.can_touch == 1) {
                                e.stopPropagation();
                        } else {
                                self.touchstart(e);
                        }
                }, false);
                view.addEventListener("touchmove", function(e){
                        if(v.can_touch == 1) {
                                e.stopPropagation();
                        } else {
                                self.touchmove(e);
                        }
                }, false);
                view.addEventListener("touchend", function(e){
                        if(v.can_touch == 1) {
                                e.stopPropagation();
                        } else {
                                self.touchup(e);
                        }
                }, false);
                view.addEventListener("touchcancel", function(e){
                        if(v.can_touch == 1) {
                                e.stopPropagation();
                        } else {
                                self.touchcancel(e);
                        }
                }, false);
        }
        return v;
}

view_helper.prototype.view_free = function(v)
{
        __remove_shared_object(v);
        v.remove();
}

view_helper.prototype.view_set_image = function(v, p)
{
        if(v.native_type == native_type.IMAGE) {
                v.custom_content.src = p;
        }
}

view_helper.prototype.view_set_font_size = function(v, s)
{
        if(v.native_type == native_type.LABEL) {
                v.style["font-size"] = s + "px";
        }
}

view_helper.prototype.view_set_text_align = function(v, a)
{
        if(v.native_type == native_type.LABEL) {
                switch (a) {
                        case 0:
                                v.style["text-align"] = "left";
                                break;
                        case 1:
                                v.style["text-align"] = "right";
                                break;
                        case 2:
                                v.style["text-align"] = "center";
                                break;
                        case 3:
                                break;
                        default:
                                break;
                }
        }
}

view_helper.prototype.view_set_text_color = function(v, r, g, b, a)
{
        if(v.native_type == native_type.LABEL) {
                v.style["color"] = "rgb(" + (r * 255) + ", " + (g * 255) + ", " + (b * 255) +")";
        }
}

view_helper.prototype.view_set_text = function(v, t)
{
        if(v.native_type == native_type.LABEL) {
                v.innerHTML = t;
                this.view_set_size(v, v.native_size.width, v.native_size.height);
        }
}

view_helper.prototype.request_alpha = function(v)
{
        var b = v.native_visible == 1 ? 1 : 0;
        var r = v.native_alpha * b;
        if(v.native_rotation.x != 0 || v.native_rotation.y != 0) {
                r *= 0.999999;
        }
        if(r > 0) {
                v.style["opacity"] = r + "";
        }
        else v.style["opacity"] = undefined;
}

view_helper.prototype.request_transform = function(v)
{
        var xx = v.native_pos.x - v.native_size.width * v.native_anchor.x;
        var yy = v.native_pos.y - v.native_size.height * v.native_anchor.y;

        var t =
                "translate3d(" + xx + "px," + yy +"px, 0) " +
                "rotateX(" + (v.native_rotation.x) + "deg) "
                + "rotateY(" + (v.native_rotation.y) + "deg) "
                + "rotateZ(" + (v.native_rotation.z) + "deg) "
                + "scale(" + v.native_scale.x + "," + v.native_scale.y + ") "
                ;
        v.style["transform"] = t;
        v.style["-webkit-transform"] = t;
        v.style["-ms-transform"] = t;
        v.style["-moz-transform"] = t;
        v.style["-o-transform"] = t;
        this.request_alpha(v);
}

view_helper.prototype.view_calculate_size = function(v)
{
        var tmp = document.createElement('div');
        tmp.style["font-size"]          = v.style["font-size"];
        tmp.style["font-family"]        = v.style["font-family"];
        tmp.style["position"]           = v.style["position"];
        tmp.style["width"]              = v.style["width"];
        tmp.style["max-width"]          = v.style["max-width"];
        tmp.style["height"]             = v.style["height"];
        tmp.innerHTML                   = v.innerHTML;
        document.getElementById("root").appendChild(tmp);
        v.native_size.height    = tmp.offsetHeight;
        tmp.remove();
}

view_helper.prototype.view_set_size = function(v, w, h)
{
        v.native_size.width     = w;
        v.native_size.height    = h;
        v.style["max-width"]    = w + "px";
        v.style["width"]        = w + "px";
        if(v.native_type == native_type.LABEL) {
                v.style["height"]    = "auto";
                this.view_calculate_size(v);
        } else {
                v.style["height"]       = h + "px";
        }
        this.view_set_position(v, v.native_pos.x, v.native_pos.y);
}

view_helper.prototype.view_set_position = function(v, x, y)
{
        v.native_pos.x = x;
        v.native_pos.y = y;
        this.request_transform(v);
}

view_helper.prototype.view_set_rotation = function(v, x, y, z)
{
        v.native_rotation.x = x;
        v.native_rotation.y = y;
        v.native_rotation.z = z;
        this.request_transform(v);
}

view_helper.prototype.view_set_anchor = function(v, x, y)
{
        v.native_anchor.x = x;
        v.native_anchor.y = y;
        var or = (x * 100) + "%" + (y * 100) + "%";
        v.style["transform-origin"] = or;
        v.style["-webkit-transform-origin"] = or;
        v.style["-moz-transform-origin"] = or;
        v.style["-o-transform-origin"] = or;
        this.view_set_position(v, v.native_pos.x, v.native_pos.y);
        this.request_transform(v);
}

view_helper.prototype.view_set_scale = function(v, x, y)
{
        v.native_scale.x = x;
        v.native_scale.y = y;
        this.request_transform(v);
}

view_helper.prototype.view_set_color = function(v, r, g, b, a)
{
        if(r >= 0) {
                v.style["background-color"] = "rgb(" + (r * 255) + ", " + (g * 255) + ", " + (b * 255) +")";
        } else {
                v.style["background-color"] = undefined;
        }
}

view_helper.prototype.view_set_border_color = function(v, r, g, b ,a)
{
        if(r >= 0) {
                v.style["border-color"] = "rgb(" + (r * 255) + ", " + (g * 255) + ", " + (b * 255) +")";
                v.style["border-width"]  = 2 + "pt";
                v.style["border-style"] = "solid";
        } else {
                v.style["border-color"] = undefined;
                v.style["border-width"]  = undefined;
                v.style["border-style"] = undefined;
        }
}

view_helper.prototype.view_set_border = function(v, b)
{
        v.style["border-radius"] = b + "px";
}

view_helper.prototype.view_set_alpha = function(v, a)
{
        v.native_alpha = a;

        this.request_alpha(v);
}

view_helper.prototype.view_set_visible = function(v, f)
{
        v.native_visible = f;
        this.view_set_alpha(v, v.native_alpha);
        if(f == 1) {
                v.style["display"] = "initial";
        } else {
                v.style["display"] = "none";
        }
}

view_helper.prototype.view_set_clip = function(v, b)
{
        if(b == 1) {
                v.style["overflow"] = "hidden";
        } else {
                v.style["overflow"] = "initital";
        }
}

view_helper.prototype.view_set_user_interaction = function(v, e)
{
        v.can_touch = e;
        if(v.can_touch == 1) {
                v.style["pointer-events"] = "visible";
        } else {
                v.style["pointer-events"] = "none";
        }
}

view_helper.prototype.view_remove_from_parent = function(v)
{
        v.remove();
}

view_helper.prototype.view_add_child = function(v, c)
{
        c.remove();
        v.appendChild(c);
}

var view_helper_shared = new view_helper();

package com.manhtran.nativeui;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.PorterDuff;
import android.graphics.drawable.GradientDrawable;
import android.graphics.drawable.ShapeDrawable;
import android.provider.Settings;
import android.support.v4.view.ViewCompat;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsoluteLayout;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;

/**
 * Created by Manh Tran on 5/25/17.
 */

public class CustomFunction {

    public static boolean resized = false;

    static HashMap<String, Bitmap> bitmap_cache = new HashMap<String, Bitmap>();

    public static void setBitmap(CustomImageView view, String path)
    {
        view.set_bitmap(bitmap_cache.get(path));
    }

    public static void loadBitmap(Context context, String path, String full_path, int type)
    {
        if(type == 0) {
            /*
             * assets
             */
            AssetManager assetManager = context.getAssets();
            if(full_path.startsWith("assets/")) {
                full_path = full_path.replace("assets/", "");
            }

            InputStream istr;
            Bitmap bitmap = null;
            try {
                istr = assetManager.open(full_path);
                bitmap = BitmapFactory.decodeStream(istr);
            } catch (IOException e) {
            }

            if(bitmap != null) {
                bitmap_cache.put(path, bitmap);
            }
        } else if(type == 1){
            /*
             * local
             */
            File imgFile = new  File(full_path);

            if(imgFile.exists()) {
                Bitmap bitmap = BitmapFactory.decodeFile(imgFile.getAbsolutePath());
                if(bitmap != null) {
                    bitmap_cache.put(path, bitmap);
                }
            }
        } else {
            /*
             * sd card
             */
        }
    }

    public static void clearBitmap(String path)
    {

        bitmap_cache.remove(path);
    }

    public static int[] convertPoint(View fromView, int x, int y, View toView){
        float d     = fromView.getContext().getResources().getDisplayMetrics().density;

        int[] fromCoord = new int[2];
        int[] toCoord = new int[2];
        fromView.getLocationOnScreen(fromCoord);
        toView.getLocationOnScreen(toCoord);

        x = fromCoord[0] - toCoord[0] + (int)(x * d * fromView.getScaleX());
        y = fromCoord[1] - toCoord[1] + (int)(y * d * fromView.getScaleY());

        x /= d;
        y /= d;

        return new int[]{x, y};
    }

    public static void requestView(CustomSharedView view)
    {
        try {
            CustomSharedView p = (CustomSharedView) view.getParent();
            if(p != null) {
                if(p.dirty) {
                    view.dirty = true;
                } else {
                    view.requestLayout();
                    view.dirty = true;
                }
            } else {
                if(!view.dirty) {
                    view.requestLayout();
                    view.dirty = true;
                }
            }
        } catch (Exception e) {

        }
    }

    public static void setPosition(CustomSharedView view, float x, float y)
    {
        float d                             = view.getContext().getResources().getDisplayMetrics().density;
        AbsoluteLayout.LayoutParams params = (AbsoluteLayout.LayoutParams) view.getLayoutParams();
        view.pos_x = (int) (x * d );
        view.pos_y = (int) (y * d );
        params.x = (int) (view.pos_x - params.width * view.anchor_x);
        params.y = (int) (view.pos_y - params.height * view.anchor_y);
        requestView(view);
    }

    public static void setSize(CustomSharedView view, float width, float height)
    {
        float d                             = view.getContext().getResources().getDisplayMetrics().density;
        width += 0.35 * d;
        height += 0.35 * d;
        AbsoluteLayout.LayoutParams params  = (AbsoluteLayout.LayoutParams) view.getLayoutParams();
        int w                      = (int)(width * d);
        int h                       = (int)(height * d);
        if(w != params.width || h != params.height) {
            params.width                        = w;
            params.height                       = h;
            params.x                            = (int)(view.pos_x - params.width * view.anchor_x);
            params.y                            = (int)(view.pos_y - params.height * view.anchor_y);
            view.onChangeSize(params.width, params.height);
            requestView(view);
        } else {

        }
    }

    public static void setRotation(CustomSharedView view, float rotateX, float rotateY, float rotateZ)
    {
        view.setRotation(rotateZ);
        view.setRotationX(rotateX);
        view.setRotationY(rotateY);

        requestView(view);
    }

    public static void setAnchor(CustomSharedView view, float x, float y)
    {
        view.anchor_x = x;
        view.anchor_y = y;
        view.resetPivot();

        AbsoluteLayout.LayoutParams params  = (AbsoluteLayout.LayoutParams) view.getLayoutParams();
        params.x                            = (int)(view.pos_x - params.width * view.anchor_x);
        params.y                            = (int)(view.pos_y - params.height * view.anchor_y);
        requestView(view);
    }

    public static void setScale(CustomSharedView view, float sx, float sy)
    {
        view.setScaleX(sx);
        view.setScaleY(sy);

        requestView(view);
    }

    public static void setColor(CustomSharedView view, float r, float g, float b, float a,
                                float br, float bg, float bb, float ba)
    {
        if(r >= 0 && br >= 0) {
            view.setBackgroundColor(Color.TRANSPARENT);
            GradientDrawable shape = new GradientDrawable();
            shape.setShape(GradientDrawable.RECTANGLE);
            shape.setCornerRadii(new float[] { view.r1, view.r1, view.r2, view.r2, view.r3, view.r3, view.r4, view.r4 });
            shape.setColor(Color.argb((int)(a * 255), (int)(r * 255), (int)(g * 255), (int)(b * 255)));
            shape.setStroke(1, Color.argb((int)(ba * 255), (int)(br * 255), (int)(bg * 255), (int)(bb * 255)));
            view.setBackgroundDrawable(shape);
        } else if(r >= 0) {
            view.setBackgroundColor(Color.TRANSPARENT);
            GradientDrawable shape = new GradientDrawable();
            shape.setShape(GradientDrawable.RECTANGLE);
            shape.setCornerRadii(new float[] { view.r1, view.r1, view.r2, view.r2, view.r3, view.r3, view.r4, view.r4 });
            shape.setColor(Color.argb((int)(a * 255), (int)(r * 255), (int)(g * 255), (int)(b * 255)));
//            shape.setStroke(3, Color.argb(0, 0, 0, 0));
            view.setBackgroundDrawable(shape);
        } else if(br >= 0) {
            view.setBackgroundColor(Color.TRANSPARENT);
            GradientDrawable shape = new GradientDrawable();
            shape.setShape(GradientDrawable.RECTANGLE);
            shape.setCornerRadii(new float[] { view.r1, view.r1, view.r2, view.r2, view.r3, view.r3, view.r4, view.r4 });
//            shape.setColor(Color.argb(0, 0, 0, 0));
            shape.setStroke(1, Color.argb((int)(ba * 255), (int)(br * 255), (int)(bg * 255), (int)(bb * 255)));
            view.setBackgroundDrawable(shape);
        } else {
            view.setBackgroundColor(Color.TRANSPARENT);
            view.setBackgroundDrawable(null);
        }
        view.dirty = true;
    }

    public static void setColorNull(CustomSharedView view)
    {
        view.setBackgroundDrawable(null);
        view.setBackgroundColor(Color.TRANSPARENT);
        view.dirty = true;
    }

    public static void setAlpha(CustomSharedView view, float a)
    {
        if(a < 1) {
            ViewCompat.setLayerType(view, ViewCompat.LAYER_TYPE_HARDWARE, null);
        } else {
            ViewCompat.setLayerType(view, ViewCompat.LAYER_TYPE_NONE, null);
        }
        view.setAlpha(a);
        view.dirty = true;
    }

    public static void setVisible(CustomSharedView view, int visible)
    {
        view.setVisibility(visible == 1 ? View.VISIBLE : View.INVISIBLE);
        view.dirty = true;
    }

    public static void removeFromParent(AbsoluteLayout view)
    {
        ViewGroup lp                        = (ViewGroup) view.getParent();
        if(lp != null) {
            lp.removeView(view);
        }
    }

    public static void addChild(AbsoluteLayout parent, AbsoluteLayout child)
    {
        /*
         * remove child from previous parent
         */
        ViewGroup lp                        = (ViewGroup) child.getParent();
        if(lp != null) {
            lp.removeView(child);
        }

        parent.addView(child);
    }

    public static void setTouchEnabled(CustomSharedView view, int enabled)
    {
        view.can_touch = enabled;
        view.onChangeCanTouch();
    }

    public static void setClip(CustomSharedView view, int enabled, float r1, float r2, float r3, float r4)
    {
        float d         = view.getContext().getResources().getDisplayMetrics().density;
        view.clip       = enabled == 1 ? true : false;
        view.r1         = r1 * d;
        view.r2         = r2 * d;
        view.r3         = r3 * d;
        view.r4         = r4 * d;
        view.dirty = true;
    }

    public static native int touchBeganJNI(long ptr, int index, float x, float y, float sx, float sy);
    public static native void touchMovedJNI(long ptr, int index, float x, float y, float sx, float sy);
    public static native void touchEndedJNI(long ptr, int index, float x, float y, float sx, float sy);
    public static native void touchCancelledJNI(long ptr, int index, float x, float y, float sx, float sy);
    public static native void textDoneJNI(long ptr);

    public static native void viewResizeJNI(long ptr, float width, float height);
}

package com.manhtran.nativeui;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;
import android.support.v4.view.MotionEventCompat;
import android.support.v4.view.ViewCompat;
import android.support.v4.view.ViewConfigurationCompat;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewConfiguration;
import android.widget.AbsoluteLayout;

import com.example.apple.myapplication.R;

/**
 * Created by Manh Tran on 5/25/17.
 */

public class CustomSharedView extends AbsoluteLayout{

    public View touch_view = null;
    public boolean view_to_dispatch = false;
    public int  can_touch;
    public long native_ptr;
    public boolean clip;
    float r1, r2, r3, r4;
    public float anchor_x, anchor_y;
    public int pos_x, pos_y;
    public boolean dirty = true;

    private Path path = new Path();
    private RectF rect = new RectF();

    float wt_x, wt_y;
    float ot_x, ot_y;

    public CustomSharedView(long ptr, Context context) {
        super(context);

        native_ptr  = ptr;
        can_touch = 0;
        clip = false;
        setLayoutParams(new LayoutParams(0,0,0,0));
        setClipChildren(false);
        anchor_x = 0.5f;
        anchor_y = 0.5f;
        pos_x = 0;
        pos_y = 0;
        r1 = r2 = r3 = r4 = 0;
    }

    protected void onChangeCanTouch()
    {

    }

    protected void onChangeSize(int width, int height) {

    }

    public void resetPivot()
    {
        setPivotX(anchor_x * getWidth());
        setPivotY(anchor_y * getHeight());
    }

    public void composeRoundedRectPath(RectF rect, float topLeftDiameter, float topRightDiameter,float bottomRightDiameter, float bottomLeftDiameter){
        topLeftDiameter = topLeftDiameter < 0 ? 0 : topLeftDiameter;
        topRightDiameter = topRightDiameter < 0 ? 0 : topRightDiameter;
        bottomLeftDiameter = bottomLeftDiameter < 0 ? 0 : bottomLeftDiameter;
        bottomRightDiameter = bottomRightDiameter < 0 ? 0 : bottomRightDiameter;

        path.moveTo(rect.left + topLeftDiameter/2 ,rect.top);
        path.lineTo(rect.right - topRightDiameter/2,rect.top);
        path.quadTo(rect.right, rect.top, rect.right, rect.top + topRightDiameter/2);
        path.lineTo(rect.right ,rect.bottom - bottomRightDiameter/2);
        path.quadTo(rect.right ,rect.bottom, rect.right - bottomRightDiameter/2, rect.bottom);
        path.lineTo(rect.left + bottomLeftDiameter/2,rect.bottom);
        path.quadTo(rect.left,rect.bottom,rect.left, rect.bottom - bottomLeftDiameter/2);
        path.lineTo(rect.left,rect.top + topLeftDiameter/2);
        path.quadTo(rect.left,rect.top, rect.left + topLeftDiameter/2, rect.top);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        path.reset();
        rect.set(0.5f, 0.5f, w-1.0f, h-1.0f);
        float bias = 2.5f;
        composeRoundedRectPath(rect, r1 * bias, r2 * bias, r3 * bias, r4 * bias);
        path.close();
        resetPivot();
    }

    @Override
    protected void dispatchDraw(Canvas canvas) {
        if(clip) {
            int save = canvas.save();
            canvas.clipPath(path);
            super.dispatchDraw(canvas);
            canvas.restoreToCount(save);
        } else {
            super.dispatchDraw(canvas);
        }
        dirty = false;
    }

    public static native Object searchTouchViewJNI(float sx, float sy);


    public boolean view_touch_dispatch(MotionEvent event)
    {
        boolean result = true;

        if(touch_view != null) {
            touch_view.onTouchEvent(event);

            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    touch_view = null;
                    break;
                default:
                    break;
            }

        } else {
            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                {
                    float d  = getContext().getResources().getDisplayMetrics().density;
                    float screenX = event.getRawX();
                    float screenY = event.getRawY() - 25;
                    CustomSharedView v = (CustomSharedView) searchTouchViewJNI(screenX / d, screenY / d);
                    if(v != null) {
                        if(v instanceof  CustomImageView) {
                            touch_view = ((CustomImageView)v).content;
                        } else if(v instanceof  CustomLabel) {
                            touch_view = ((CustomLabel)v).content;
                        } else if(v instanceof  CustomTextField) {
                            touch_view = ((CustomTextField)v).content;
                            touch_view.requestFocus();
                        } else if(v instanceof  CustomTextView) {
                            touch_view = ((CustomTextView)v).content;
                            touch_view.requestFocus();
                        } else {
                            touch_view = v;
                        }

                        touch_view.onTouchEvent(event);
                    }
                }
                    break;
                case MotionEvent.ACTION_MOVE:
                    result = true;
                    break;
                case MotionEvent.ACTION_UP:
                    touch_view = null;
                    result = true;
                    break;
                case MotionEvent.ACTION_CANCEL:
                    touch_view = null;
                    result = true;
                    break;
                default:
                    break;
            }

        }

        return result;
    }

    public boolean view_touch_used(MotionEvent event)
    {
        boolean result = false;
        if(can_touch == 1) {
            float screenX = event.getRawX();
            float screenY = event.getRawY();
            float viewX = event.getX();
            float viewY = event.getY();

            float d  = getContext().getResources().getDisplayMetrics().density;
            switch (event.getActionMasked()) {
                case MotionEvent.ACTION_DOWN:
                    wt_x = screenX;
                    wt_y = screenY;
                    ot_x = viewX;
                    ot_y = viewY;
                    result = CustomFunction.touchBeganJNI(native_ptr, 0, viewX / d, viewY / d, screenX / d, screenY/d) == 1;
                    break;
                case MotionEvent.ACTION_MOVE:
                    viewX = ot_x + screenX - wt_x;
                    viewY = ot_y + screenY - wt_y;
                    CustomFunction.touchMovedJNI(native_ptr, 0, viewX / d, viewY / d, screenX / d, screenY/d);
                    result = true;
                    break;
                case MotionEvent.ACTION_UP:
                    viewX = ot_x + screenX - wt_x;
                    viewY = ot_y + screenY - wt_y;
                    CustomFunction.touchEndedJNI(native_ptr, 0, viewX / d, viewY / d, screenX / d, screenY/d);
                    result = true;
                    break;
                case MotionEvent.ACTION_CANCEL:
                    viewX = ot_x + screenX - wt_x;
                    viewY = ot_y + screenY - wt_y;
                    CustomFunction.touchCancelledJNI(native_ptr, 0,viewX / d, viewY / d, screenX / d, screenY/d);
                    result = true;
                    break;
                default:
                    break;
            }
        }
        return result;
    }



    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if(view_to_dispatch) {
            return view_touch_dispatch(event);
        } else {
            return view_touch_used(event);
        }
    }
}

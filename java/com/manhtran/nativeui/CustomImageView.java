package com.manhtran.nativeui;

import android.content.Context;
import android.graphics.Bitmap;
import android.support.v4.view.ViewCompat;
import android.view.MotionEvent;
import android.widget.AbsoluteLayout;
import android.widget.ImageView;

import com.example.apple.myapplication.R;

/**
 * Created by Manh Tran on 5/25/17.
 */

public class CustomImageView extends CustomSharedView {

    public static CustomImageView create(long ptr, Context context) {
        CustomImageView p    = new CustomImageView(ptr, context);
        return p;
    }

    public InnerImageView       content;

    public CustomImageView(long ptr, Context context) {
        super(ptr, context);

        content = new InnerImageView(context);
        content.setScaleType(ImageView.ScaleType.FIT_XY);
        AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
                AbsoluteLayout.LayoutParams.MATCH_PARENT,
                AbsoluteLayout.LayoutParams.MATCH_PARENT,
                0,
                0
        );
        addViewInLayout(content, 0, params);
    }

    public void set_bitmap(Bitmap bitmap)
    {
        content.setImageBitmap(bitmap);
    }

    public void load_image() {

    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return false;
    }

    public class InnerImageView extends android.support.v7.widget.AppCompatImageView {

        public InnerImageView(Context context) {

            super(context);
//            ViewCompat.setLayerType(this, ViewCompat.LAYER_TYPE_HARDWARE, null);
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
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
    }
}

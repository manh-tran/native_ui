package com.manhtran.nativeui;

import android.content.Context;
import android.graphics.PointF;
import android.view.View;

import com.example.apple.myapplication.R;

/**
 * Created by Manh Tran on 5/25/17.
 */

public class CustomView extends CustomSharedView {

    public static CustomView create(long ptr, Context context) {
        CustomView p    = new CustomView(ptr, context);
        return p;
    }

    public CustomView(long ptr, Context context) {

        super(ptr, context);
    }

    /*
     * allow touch children out of view
     */
    protected boolean isTransformedTouchPointInView(float x, float y, View child, PointF outLocalPoint) {
        return true;
    }

}

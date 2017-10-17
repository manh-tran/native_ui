package com.manhtran.nativeui;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Typeface;
import android.support.v4.view.ViewCompat;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.MotionEvent;
import android.widget.AbsoluteLayout;
import android.widget.TextView;

/**
 * Created by Manh Tran on 5/25/17.
 */

public class CustomLabel extends CustomSharedView {

    public static CustomLabel create(long ptr, Context context) {
        CustomLabel p    = new CustomLabel(ptr, context);
        return p;
    }

    public InnerLabel content;

    public CustomLabel(long ptr, Context context) {
        super(ptr, context);

        content = new InnerLabel(context);
        AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
                AbsoluteLayout.LayoutParams.MATCH_PARENT,
                AbsoluteLayout.LayoutParams.WRAP_CONTENT,
                0,
                0
        );
        addViewInLayout(content, 0, params);
        content.setTextColor(Color.BLACK);
    }

    public void setFontSize(float size) {
        content.setTextSize(size);
        content.requestLayout();
    }

    public void setMultiline(int enable) {
        content.setSingleLine(enable == 1 ? false : true);
        content.requestLayout();
    }

    public void setText(String text) {
        content.setText(text);
        content.requestLayout();
    }

    public void setTextAlignment(int align) {
        switch (align) {
            case 0:
                content.justified = false;
                content.setGravity(Gravity.LEFT);
                break;
            case 1:
                content.justified = false;
                content.setGravity(Gravity.RIGHT);
                break;
            case 2:
                content.justified = false;
                content.setGravity(Gravity.CENTER);
                break;
            case 3:
                content.justified = true;
                break;
            default:
                break;
        }
    }

    @Override
    protected void onChangeSize(int width, int height) {
        AbsoluteLayout.LayoutParams params  = (AbsoluteLayout.LayoutParams) content.getLayoutParams();
        params.height = content.getLineHeight()  * content.getLineCount() + 10;
        requestLayout();

        int widthMeasureSpec = MeasureSpec.makeMeasureSpec(width, MeasureSpec.AT_MOST);
        int heightMeasureSpec = MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED);
        content.measure(widthMeasureSpec, heightMeasureSpec);

//        System.out.println("native ui change size: " + params.height +", "+ content.getMeasuredHeight());
    }

    public int calculateHeight()
    {
        AbsoluteLayout.LayoutParams params  = (AbsoluteLayout.LayoutParams) getLayoutParams();

        int widthMeasureSpec = MeasureSpec.makeMeasureSpec(params.width, MeasureSpec.AT_MOST);
        int heightMeasureSpec = MeasureSpec.makeMeasureSpec(0, MeasureSpec.UNSPECIFIED);
        content.measure(widthMeasureSpec, heightMeasureSpec);

        float d  = getContext().getResources().getDisplayMetrics().density;

        return (int)((content.getMeasuredHeight() + 10) / d);
    }

    public void setTextColor(float r, float g, float b, float a) {
        content.setTextColor(Color.argb((int)(a * 255), (int)(r * 255), (int)(g * 255), (int)(b * 255)));
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return false;
    }

    public class InnerLabel extends android.support.v7.widget.AppCompatTextView {

        int last_width, last_height;
        boolean justified;
        private int mLineY;
        private int mViewWidth;

        public InnerLabel(Context context) {

            super(context);
            last_width = 0;
            last_height = 0;
            justified = false;
            //ViewCompat.setLayerType(this, ViewCompat.LAYER_TYPE_HARDWARE, null);
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

        @Override
        protected void onDraw(Canvas canvas) {
            if(justified) {
                onDrawJustified(canvas);
            } else {
                super.onDraw(canvas);
            }
        }

        private void onDrawJustified(Canvas canvas)
        {
            TextPaint paint = getPaint();
            paint.setColor(getCurrentTextColor());
            paint.drawableState = getDrawableState();
            mViewWidth = getMeasuredWidth();
            String text = getText().toString();
            mLineY = 0;
            mLineY += getTextSize();
            Layout layout = getLayout();
            for (int i = 0; i < layout.getLineCount(); i++) {
                int lineStart = layout.getLineStart(i);
                int lineEnd = layout.getLineEnd(i);
                String line = text.substring(lineStart, lineEnd);

                float width = StaticLayout.getDesiredWidth(text, lineStart, lineEnd, getPaint());
                if (needScale(line,i)) {
                    drawScaledText(canvas, lineStart, line, width);
                } else {
                    canvas.drawText(line, 0, mLineY, paint);
                }

                mLineY += getLineHeight();
            }
        }

        private void drawScaledText(Canvas canvas, int lineStart, String line, float lineWidth) {
            float x = 0;
            if (isFirstLineOfParagraph(lineStart, line)) {
                String blanks = "  ";
                canvas.drawText(blanks, x, mLineY, getPaint());
                float bw = StaticLayout.getDesiredWidth(blanks, getPaint());
                x += bw;

                line = line.substring(3);
            }

            float d = (mViewWidth - lineWidth) / line.length() - 1;
            for (int i = 0; i < line.length(); i++) {
                String c = String.valueOf(line.charAt(i));
                float cw = StaticLayout.getDesiredWidth(c, getPaint());
                canvas.drawText(c, x, mLineY, getPaint());
                x += cw + d;
            }
        }

        private boolean isFirstLineOfParagraph(int lineStart, String line) {
            return line.length() > 3 && line.charAt(0) == ' ' && line.charAt(1) == ' ';
        }

        private boolean needScale(String line,int lineNumber) {
            Layout layout = getLayout();
            if (line.length() == 0 || layout.getLineCount() == 1 || lineNumber == (layout.getLineCount() - 1)) {
                return false;
            } else {
                return line.charAt(line.length() - 1) != '\n';
            }
        }

    }
}

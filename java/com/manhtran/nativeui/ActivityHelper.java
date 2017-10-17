package com.manhtran.nativeui;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Looper;
import android.widget.AbsoluteLayout;

import com.example.apple.myapplication.R;

/**
 * Created by Manh Tran on 7/1/2017.
 */

public class ActivityHelper {

    private int mInterval = 16;
    private Handler mHandler;
    private boolean resized = false;
    CustomView root = null;
    AbsoluteLayout layout = null;
    Activity ctx;

    public void onCreate(Activity activity)
    {
        ctx = activity;
        layout = (AbsoluteLayout) ctx.findViewById(R.id.root);

        root = initNative();
        AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
                AbsoluteLayout.LayoutParams.MATCH_PARENT,
                AbsoluteLayout.LayoutParams.MATCH_PARENT,
                0,
                0
        );
        layout.addView(root, params);
        layout.forceLayout();

        mHandler = new Handler(Looper.getMainLooper());
        startRepeatingTask();

        root.post(new Runnable() {
            @Override
            public void run() {
                resize_root();
            }
        });
    }

    public void onWindowFocusChanged(boolean hasFocus) {
        resize_root();
    }

    public void onDestroy() {
        stopRepeatingTask();
    }

    Runnable mStatusChecker = new Runnable() {
        @Override
        public void run() {
            if(resized) onLoopJNI();
            mHandler.postDelayed(mStatusChecker, mInterval);
        }
    };

    private CustomView initNative() {
        String apkFilePath = null;

        ApplicationInfo appInfo = null;
        PackageManager packMgmr = ctx.getPackageManager();
        try {
            appInfo = packMgmr.getApplicationInfo(ctx.getPackageName(), 0);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            throw new RuntimeException("Unable to locate assets, aborting...");
        }
        apkFilePath = appInfo.sourceDir;
        return initNativeJNI(apkFilePath, ctx.getFilesDir().getAbsolutePath(), ctx);
    }

    void resize_root()
    {
        if(root != null) {
            layout.requestLayout();
            root.requestLayout();
            if(layout.getWidth() > 0) {
                float d   = ctx.getResources().getDisplayMetrics().density;
                onResizeJNI((int)(layout.getWidth() / d), (int)(layout.getHeight() / d));
                resized = true;
                CustomFunction.resized = true;
            }
        }
    }

    void startRepeatingTask() {
        mStatusChecker.run();
    }

    void stopRepeatingTask() {
        mHandler.removeCallbacks(mStatusChecker);
    }

    public native CustomView initNativeJNI(String asset, String local, Context context);

    public native void onResizeJNI(int width, int height);

    public native void onLoopJNI();
}

package com.hub7a4.ffmenu;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.IBinder;
import android.provider.Settings;
import android.view.Gravity;
import android.view.WindowManager;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class OverlayService extends Service {
    static {
        System.loadLibrary("hub7a4");
    }

    private WindowManager wm;
    private GLSurfaceView glView;

    public static native void nativeInit(AssetManager mgr, int width, int height);
    public static native void nativeRender();
    public static native void nativeTouch(int action, float x, float y);
    public static native boolean nativeWantCaptureMouse();

    @Override
    public void onCreate() {
        super.onCreate();
        if (!Settings.canDrawOverlays(this)) {
            stopSelf();
            return;
        }
        createNotification();

        wm = (WindowManager) getSystemService(WINDOW_SERVICE);
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
            WindowManager.LayoutParams.MATCH_PARENT,
            WindowManager.LayoutParams.MATCH_PARENT,
            WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                | WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
            PixelFormat.TRANSLUCENT
        );
        params.gravity = Gravity.TOP | Gravity.START;

        glView = new GLSurfaceView(this);
        glView.setEGLContextClientVersion(3);
        glView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        glView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
        glView.setZOrderOnTop(true);

        glView.setRenderer(new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl, EGLConfig config) {
                nativeInit(getAssets(), glView.getWidth(), glView.getHeight());
            }

            @Override
            public void onSurfaceChanged(GL10 gl, int width, int height) {
                nativeInit(getAssets(), width, height);
            }

            @Override
            public void onDrawFrame(GL10 gl) {
                nativeRender();
            }
        });

        glView.setOnTouchListener((v, event) -> {
            nativeTouch(event.getActionMasked(), event.getX(), event.getY());
            return nativeWantCaptureMouse();
        });

        wm.addView(glView, params);
    }

    private void createNotification() {
        String id = "hub7a4_channel";
        NotificationChannel ch = new NotificationChannel(id, "7A4 Hub Overlay", NotificationManager.IMPORTANCE_LOW);
        getSystemService(NotificationManager.class).createNotificationChannel(ch);
        Notification n = new Notification.Builder(this, id)
            .setContentTitle("7A4 FF Hub Dang Chay")
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .build();
        startForeground(1, n);
    }

    @Override
    public void onDestroy() {
        if (wm != null && glView != null) wm.removeView(glView);
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}

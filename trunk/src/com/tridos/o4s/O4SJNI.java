package com.tridos.o4s;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class O4SJNI extends GLSurfaceView implements Renderer {

    boolean init = false;
    boolean paused = false;
    int leftId = Integer.MIN_VALUE;
    int rightId = Integer.MIN_VALUE;
    int leftButton, rightButton;
    long lastFrameTime;
    // define buttons
    Button left = new Button(0, 0.761f, 0.112f, 1);
    Button right = new Button(0.156f, 0.761f, 0.263f, 1);
    Button n2o = new Button(0.886f, 0.221f, 1, 0.454f);
    Button up = new Button(0.886f, 0.452f, 1, 0.695f);
    Button down = new Button(0.886f, 0.763f, 1, 1);
    private static float center = 0.625f;

    public O4SJNI(Context context) {
        super(context);
        setEGLContextClientVersion(2);
    }

    @Override
    public boolean onTouchEvent(final MotionEvent e) {
        super.onTouchEvent(e);
        // simple click on buttons
        if (e.getAction() == MotionEvent.ACTION_DOWN) {
            O4SJNI.nativeClick((int) e.getX(), (int) e.getY());
        }

        int actM = e.getAction() & MotionEvent.ACTION_MASK;
        int actS = e.getAction() >> MotionEvent.ACTION_POINTER_ID_SHIFT;
        try {

            // release left/right
            if (((actM == MotionEvent.ACTION_UP) & (e.getPointerId(0) == leftId))
                    | ((actM == MotionEvent.ACTION_POINTER_UP) & (actS == leftId))) {
                nativeKeyUp(100);
                nativeKeyUp(102);
                leftButton = 0;
                leftId = Integer.MIN_VALUE;
            }

            // release up/down/n2o
            if (((actM == MotionEvent.ACTION_UP) & (e.getPointerId(0) == rightId))
                    | ((actM == MotionEvent.ACTION_POINTER_UP) & (actS == rightId))) {
                nativeKeyUp(101);
                nativeKeyUp(103);
                nativeKeyUp(160);
                rightButton = 0;
                rightId = Integer.MIN_VALUE;
            }

            // apply id to pointer in single touch
            if (actM == MotionEvent.ACTION_DOWN) {
                if (e.getX() / (float) getWidth() <= center) {
                    leftId = e.getPointerId(0);
                } else {
                    rightId = e.getPointerId(0);
                }
            }

            // apply id to pointer in multi touch
            if (actM == MotionEvent.ACTION_POINTER_DOWN) {
                if (e.getX(e.findPointerIndex(actS)) / (float) getWidth() <= center) {
                    leftId = actS;
                } else {
                    rightId = actS;
                }
            }
        } catch (Exception ex) {
        }

        // left-right buttons
        try {
            if (leftId != Integer.MIN_VALUE) {
                float x = e.getX(e.findPointerIndex(leftId))
                        / (float) getWidth();
                float y = e.getY(e.findPointerIndex(leftId))
                        / (float) getHeight();

                if (left.isInside(x, y)) {
                    if (leftButton != 1) {
                        nativeKeyUp(102);
                        nativeKey(100);
                        leftButton = 1;
                    }
                } else if (right.isInside(x, y)) {
                    if (leftButton != 2) {
                        nativeKeyUp(100);
                        nativeKey(102);
                        leftButton = 2;
                    }
                } else {
                    if (leftButton != 0) {
                        nativeKeyUp(100);
                        nativeKeyUp(102);
                        leftButton = 0;
                    }
                }
            }
        } catch (Exception ex) {
            nativeKeyUp(100);
            nativeKeyUp(102);
            leftButton = 0;
            leftId = Integer.MIN_VALUE;
        }

        // up-down buttons
        try {
            if (rightId != Integer.MIN_VALUE) {
                float x = e.getX(e.findPointerIndex(rightId))
                        / (float) getWidth();
                float y = e.getY(e.findPointerIndex(rightId))
                        / (float) getHeight();
                if (up.isInside(x, y)) {
                    if (rightButton != 1) {
                        nativeKeyUp(103);
                        nativeKey(101);
                        nativeKeyUp(160);
                        rightButton = 1;
                    }
                } else if (down.isInside(x, y)) {
                    if (rightButton != 2) {
                        nativeKeyUp(101);
                        nativeKey(103);
                        nativeKeyUp(160);
                        rightButton = 2;
                    }
                } else if (n2o.isInside(x, y)) {
                    if (rightButton != 3) {
                        nativeKeyUp(101);
                        nativeKeyUp(103);
                        nativeKey(160);
                        rightButton = 3;
                    }
                } else if (rightButton != 0) {
                    nativeKeyUp(101);
                    nativeKeyUp(103);
                    nativeKeyUp(160);
                    rightButton = 0;
                }
            }
        } catch (Exception ex) {
            nativeKeyUp(101);
            nativeKeyUp(103);
            nativeKeyUp(160);
            rightButton = 0;
            rightId = Integer.MIN_VALUE;
        }

        return true;
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        if (!init) {
            String apkFilePath = null;
            ApplicationInfo appInfo = null;
            PackageManager packMgmr = O4SActivity.mO4SActivity
                    .getPackageManager();
            try {
                appInfo = packMgmr.getApplicationInfo("com.tridos.o4s", 0);
            } catch (Exception e) {
                e.printStackTrace();
                throw new RuntimeException("Unable to locate assets, aborting...");
            }
            apkFilePath = appInfo.sourceDir;
            nativeInit(apkFilePath);
        }
        init = true;
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        nativeResize(w, h);
    }

    public void onDrawFrame(GL10 gl) {
        if (!paused) {
            Thread.currentThread().setPriority(Thread.MAX_PRIORITY);
            nativeLoop();
            long gap = System.currentTimeMillis() - lastFrameTime;
            if (50 - gap > 5) {
                try {
                    Thread.sleep(50 - gap);
                } catch (Exception ex) {
                }
            }
            lastFrameTime = System.currentTimeMillis();
        }
    }

    class Button {

        float x1, y1, x2, y2;

        public Button(float x1, float y1, float x2, float y2) {
            this.x1 = x1;
            this.y1 = y1;
            this.x2 = x2;
            this.y2 = y2;
        }

        public boolean isInside(float x, float y) {
            if ((x >= x1) & (y >= y1) & (x <= x2) & (y <= y2)) {
                return true;
            } else {
                return false;
            }
        }
    }

    static {
        System.loadLibrary("open4speed");
    }

    public static native void nativeBack();

    private static native void nativeClick(int x, int y);

    private static native void nativeInit(String str);

    public static native void nativeKey(int code);

    public static native void nativeKeyUp(int code);

    private static native void nativeLoop();

    private static native void nativeResize(int w, int h);
}
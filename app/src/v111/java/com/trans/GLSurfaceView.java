package com.trans;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GLSurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    public static int DEBUG_CHECK_GL_ERROR;
    public static int DEBUG_LOG_GL_CALLS;
    private static boolean DRAW_TWICE_AFTER_SIZE_CHANGED;
    private static boolean LOG_ATTACH_DETACH;
    private static boolean LOG_EGL;
    private static boolean LOG_PAUSE_RESUME;
    private static boolean LOG_RENDERER;
    private static boolean LOG_RENDERER_DRAW_FRAME;
    private static boolean LOG_SURFACE;
    private static boolean LOG_THREADS;
    public static int RENDERMODE_CONTINUOUSLY = 1;
    public static int RENDERMODE_WHEN_DIRTY;
    public static int SWAPMODE_AUTOMATICALLY = 1;
    public static int SWAPMODE_MANUALLY;
    private static String TAG = "GLSurfaceView";
    private static GLSurfaceView$GLThreadManager sGLThreadManager = new GLSurfaceView$GLThreadManager(0);
    private int mDebugFlags;
    private boolean mDetached;
    private GLSurfaceView$EGLConfigChooser mEGLConfigChooser;
    private int mEGLContextClientVersion;
    private GLSurfaceView$EGLContextFactory mEGLContextFactory;
    private GLSurfaceView$EGLWindowSurfaceFactory mEGLWindowSurfaceFactory;
    protected GLSurfaceView$GLThread mGLThread;
    private GLSurfaceView$GLWrapper mGLWrapper;
    private GLSurfaceView$Renderer mRenderer;
    private boolean mSizeChanged;
    boolean mSurfaceCreated;

    public GLSurfaceView(Context p0) {
        super(p0);
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
        getHolder().setFormat(android.graphics.PixelFormat.RGBA_8888);
        getHolder().addCallback(this);
    }

    public GLSurfaceView(Context p0, AttributeSet p1) {
        super(p0, p1);
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_GPU);
        getHolder().setFormat(android.graphics.PixelFormat.RGBA_8888);
        getHolder().addCallback(this);
    }

    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "surfaceCreated");
        mSurfaceCreated = true;
        if (mGLThread != null) {
            mGLThread.surfaceCreated();
        }
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        Log.i(TAG, "surfaceChanged: " + w + "x" + h);
        if (mGLThread != null) {
            mGLThread.onWindowResize(w, h);
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "surfaceDestroyed");
        mSurfaceCreated = false;
        if (mGLThread != null) {
            mGLThread.surfaceDestroyed();
        }
    }

    private void checkRenderThreadState() {
        if (mGLThread != null) {
            throw new IllegalStateException("setRenderer has already been called");
        }
    }

    private void init() {
    }

    public int getDebugFlags() {
        return mDebugFlags;
    }

    public int getRenderMode() {
        if (mGLThread != null) {
            return mGLThread.getRenderMode();
        }
        return RENDERMODE_CONTINUOUSLY;
    }

    public int getSwapMode() {
        if (mGLThread != null) {
            return mGLThread.getSwapMode();
        }
        return SWAPMODE_AUTOMATICALLY;
    }

    public void handleEvents() {
        if (mGLThread != null) {
            mGLThread.handleEvents();
        }
    }

    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
    }

    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        mDetached = true;
        if (mGLThread != null) {
            mGLThread.requestExitAndWait();
        }
    }

    public void onPause() {
        if (mGLThread != null) {
            mGLThread.onPause();
        }
    }

    public void onResume() {
        if (mGLThread != null) {
            mGLThread.onResume();
        }
    }

    public void prepare() {
    }

    public void queueEvent(Runnable p0) {
        if (mGLThread != null) {
            mGLThread.queueEvent(p0);
        }
    }

    public void requestRender() {
        if (mGLThread != null) {
            mGLThread.requestRender();
        }
    }

    public void setDebugFlags(int p0) {
        mDebugFlags = p0;
    }

    public void setEGLConfigChooser(int p0, int p1, int p2, int p3, int p4, int p5) {
        setEGLConfigChooser(new GLSurfaceView$ComponentSizeChooser(this, p0, p1, p2, p3, p4, p5));
    }

    public void setEGLConfigChooser(GLSurfaceView$EGLConfigChooser p0) {
        checkRenderThreadState();
        mEGLConfigChooser = p0;
    }

    public void setEGLConfigChooser(boolean p0) {
        setEGLConfigChooser(new GLSurfaceView$SimpleEGLConfigChooser(this, p0));
    }

    public void setEGLContextClientVersion(int p0) {
        checkRenderThreadState();
        mEGLContextClientVersion = p0;
    }

    public void setEGLContextFactory(GLSurfaceView$EGLContextFactory p0) {
        checkRenderThreadState();
        mEGLContextFactory = p0;
    }

    public void setEGLWindowSurfaceFactory(GLSurfaceView$EGLWindowSurfaceFactory p0) {
        checkRenderThreadState();
        mEGLWindowSurfaceFactory = p0;
    }

    public void setGLWrapper(GLSurfaceView$GLWrapper p0) {
        mGLWrapper = p0;
    }

    public void setRenderMode(int p0) {
        if (mGLThread != null) {
            mGLThread.setRenderMode(p0);
        }
    }

    public void setRenderer(GLSurfaceView$Renderer p0) {
        checkRenderThreadState();
        mRenderer = p0;
        mGLThread = new GLSurfaceView$GLThread(this, mRenderer);
        mGLThread.start();
    }

    public void setSwapMode(int p0) {
        if (mGLThread != null) {
            mGLThread.setSwapMode(p0);
        }
    }

    public void swapBuffers() {
        if (mGLThread != null) {
            mGLThread.swapBuffers();
        }
    }
}

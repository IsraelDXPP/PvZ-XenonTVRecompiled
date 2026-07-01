package com.trans;

import java.util.ArrayList;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;
import android.util.Log;
import android.view.Surface;

class GLSurfaceView$GLThread extends Thread {
    boolean createEglContext;
    boolean createEglSurface;
    boolean doRenderNotification;
    Runnable event;
    GL10 gl;
    int h;
    private GLSurfaceView$EglHelper mEglHelper;
    private EGLDisplay mEglDisplay;
    private EGLSurface mEglSurface;
    private ArrayList<Runnable> mEventQueue;
    private boolean mExited;
    private boolean mHasSurface;
    private boolean mHaveEglContext;
    private boolean mHaveEglSurface;
    private int mHeight;
    private boolean mLostEglContext;
    private boolean mPaused;
    private boolean mRenderComplete;
    private int mRenderMode;
    private GLSurfaceView$Renderer mRenderer;
    private boolean mRequestPaused;
    private boolean mRequestRender;
    boolean mShouldExit;
    private boolean mShouldReleaseEglContext;
    private int mSwapMode;
    private boolean mWaitingForSurface;
    private int mWidth;
    boolean sizeChanged;
    GLSurfaceView this$0;
    int w;
    boolean wantRenderNotification;

    GLSurfaceView$GLThread(GLSurfaceView p0, GLSurfaceView$Renderer p1) {
        this.this$0 = p0;
        mRenderer = p1;
        mEventQueue = new ArrayList<>();
        mRenderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY;
        mSwapMode = GLSurfaceView.SWAPMODE_AUTOMATICALLY;
    }

    private void guardedRun() {
        try {
            EGL10 egl = (EGL10) EGLContext.getEGL();
            EGLDisplay display = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
            int[] vers = new int[2];
            egl.eglInitialize(display, vers);

            int[] attribs = {
                EGL10.EGL_RED_SIZE, 8,
                EGL10.EGL_GREEN_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 8,
                EGL10.EGL_DEPTH_SIZE, 24,
                EGL10.EGL_STENCIL_SIZE, 8,
                EGL10.EGL_NONE
            };
            EGLConfig[] configs = new EGLConfig[1];
            int[] numConfigs = new int[1];
            egl.eglChooseConfig(display, attribs, configs, 1, numConfigs);
            if (numConfigs[0] == 0) {
                int[] fallback = {
                    EGL10.EGL_RED_SIZE, 5,
                    EGL10.EGL_GREEN_SIZE, 6,
                    EGL10.EGL_BLUE_SIZE, 5,
                    EGL10.EGL_DEPTH_SIZE, 16,
                    EGL10.EGL_STENCIL_SIZE, 8,
                    EGL10.EGL_NONE
                };
                egl.eglChooseConfig(display, fallback, configs, 1, numConfigs);
                if (numConfigs[0] == 0) {
                    Log.e("GLThread", "no EGL config found");
                    return;
                }
            }
            EGLConfig config = configs[0];

            EGLContext context = egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, null);
            if (context == null) {
                Log.e("GLThread", "eglCreateContext failed");
                return;
            }

            while (!this$0.mSurfaceCreated && !mShouldExit) {
                try { Thread.sleep(50); } catch (InterruptedException e) {}
            }
            if (mShouldExit) return;

            Surface surface = this$0.getHolder().getSurface();
            if (surface == null || !surface.isValid()) return;
            EGLSurface eglSurface = egl.eglCreateWindowSurface(display, config, surface, null);
            if (eglSurface == null) {
                Log.e("GLThread", "eglCreateWindowSurface failed");
                return;
            }

            egl.eglMakeCurrent(display, eglSurface, eglSurface, context);

            mEglDisplay = display;
            mEglSurface = eglSurface;
            mWidth = this$0.getWidth();
            mHeight = this$0.getHeight();

            if (mRenderer != null) {
                mRenderer.onSurfaceCreated(null, config);
                mRenderer.onSurfaceChanged(null, mWidth, mHeight);
            }

            mHaveEglContext = true;
            mHaveEglSurface = true;

            while (!mShouldExit) {
                synchronized (this) {
                    while (!mEventQueue.isEmpty()) {
                        Runnable r = mEventQueue.remove(0);
                        if (r != null) {
                            r.run();
                        }
                    }
                }
                if (mRenderer != null) {
                    mRenderer.onDrawFrame(null);
                }
                if (mSwapMode == GLSurfaceView.SWAPMODE_AUTOMATICALLY) {
                    swapBuffers();
                }
            }

            egl.eglMakeCurrent(display, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
            egl.eglDestroyContext(display, context);
            egl.eglDestroySurface(display, eglSurface);
            egl.eglTerminate(display);
        } catch (Exception e) {
            Log.e("GLThread", "guardedRun exception: " + e.getMessage());
        }
    }

    public boolean ableToDraw() {
        return mHaveEglContext && mHaveEglSurface && !mPaused;
    }

    public int getRenderMode() {
        return mRenderMode;
    }

    public int getSwapMode() {
        return mSwapMode;
    }

    public void handleEvents() {
        synchronized (this) {
            while (!mEventQueue.isEmpty()) {
                Runnable r = mEventQueue.remove(0);
                if (r != null) {
                    r.run();
                }
            }
        }
    }

    public void onPause() {
        mPaused = true;
    }

    public void onResume() {
        mPaused = false;
        mRequestRender = true;
    }

    public void onWindowResize(int p0, int p1) {
        mWidth = p0;
        mHeight = p1;
        sizeChanged = true;
    }

    public void queueEvent(Runnable p0) {
        if (p0 == null) return;
        synchronized (this) {
            mEventQueue.add(p0);
            notifyAll();
        }
    }

    public void requestExitAndWait() {
        mShouldExit = true;
    }

    public void requestReleaseEglContextLocked() {
    }

    public void requestRender() {
        synchronized (this) {
            mRequestRender = true;
        }
    }

    public void run() {
        guardedRun();
    }

    public void setRenderMode(int p0) {
        mRenderMode = p0;
    }

    public void setSwapMode(int p0) {
        mSwapMode = p0;
    }

    public void swapBuffers() {
        if (mEglDisplay != null && mEglSurface != null) {
            EGL10 egl = (EGL10) EGLContext.getEGL();
            egl.eglSwapBuffers(mEglDisplay, mEglSurface);
        }
    }

    public void surfaceCreated() {
        mHasSurface = true;
        mWaitingForSurface = false;
    }

    public void surfaceDestroyed() {
        mHasSurface = false;
        mWaitingForSurface = true;
    }
}

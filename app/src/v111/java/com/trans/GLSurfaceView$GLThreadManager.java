package com.trans;

import javax.microedition.khronos.opengles.GL10;

class GLSurfaceView$GLThreadManager extends Object {
    private static String TAG = "GLThreadManager";
    private static int kGLES_20;
    private static String kMSM7K_RENDERER_PREFIX;
    private GLSurfaceView$GLThread mEglOwner;
    private boolean mGLESDriverCheckComplete;
    private int mGLESVersion;
    private boolean mGLESVersionCheckComplete;
    private boolean mMultipleGLESContextsAllowed;

    private GLSurfaceView$GLThreadManager() {
    }

    GLSurfaceView$GLThreadManager(int p0) {
    }

    private void checkGLESVersion() {
        mGLESVersionCheckComplete = true;
        mGLESVersion = kGLES_20;
        mMultipleGLESContextsAllowed = true;
    }

    public void checkGLDriver(GL10 gl) {
        if (!mGLESDriverCheckComplete) {
            mGLESDriverCheckComplete = true;
        }
    }

    public void releaseEglContextLocked(GLSurfaceView$GLThread p0) {
        mEglOwner = null;
        notifyAll();
    }

    public boolean shouldReleaseEGLContextWhenPausing() {
        return false;
    }

    public boolean shouldTerminateEGLWhenPausing() {
        return false;
    }

    public void threadExiting(GLSurfaceView$GLThread p0) {
        if (mEglOwner == p0) {
            mEglOwner = null;
        }
        notifyAll();
    }
}

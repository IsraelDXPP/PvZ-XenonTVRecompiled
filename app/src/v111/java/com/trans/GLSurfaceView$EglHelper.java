package com.trans;

import android.view.SurfaceHolder;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL;

class GLSurfaceView$EglHelper extends Object {
    EGL10 mEgl;
    EGLConfig mEglConfig;
    EGLContext mEglContext;
    EGLDisplay mEglDisplay;
    EGLSurface mEglSurface;
    GLSurfaceView this$0;

    public GLSurfaceView$EglHelper(GLSurfaceView p0) {
        this.this$0 = p0;
    }

    private void throwEglException(String p0) {
        throwEglException(p0, mEgl.eglGetError());
    }

    private void throwEglException(String p0, int p1) {
        throw new RuntimeException(p0 + ": " + p1);
    }

    public GL createSurface(SurfaceHolder p0) {
        if (mEglDisplay == null || mEglConfig == null) return null;
        if (mEglSurface != null) {
            mEgl.eglDestroySurface(mEglDisplay, mEglSurface);
            mEglSurface = null;
        }
        mEglSurface = mEgl.eglCreateWindowSurface(mEglDisplay, mEglConfig, p0, null);
        if (mEglSurface == null) return null;
        if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
            return null;
        }
        return mEglContext.getGL();
    }

    public void destroySurface() {
        if (mEglSurface != null) {
            mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
            mEgl.eglDestroySurface(mEglDisplay, mEglSurface);
            mEglSurface = null;
        }
    }

    public void finish() {
        if (mEglContext != null) {
            mEgl.eglDestroyContext(mEglDisplay, mEglContext);
            mEglContext = null;
        }
        if (mEglDisplay != null) {
            mEgl.eglTerminate(mEglDisplay);
            mEglDisplay = null;
        }
    }

    public void start() {
        mEgl = (EGL10) EGLContext.getEGL();
        mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
        int[] vers = new int[2];
        mEgl.eglInitialize(mEglDisplay, vers);
        int[] configAttribs = {
            EGL10.EGL_RED_SIZE, 8,
            EGL10.EGL_GREEN_SIZE, 8,
            EGL10.EGL_BLUE_SIZE, 8,
            EGL10.EGL_DEPTH_SIZE, 24,
            EGL10.EGL_STENCIL_SIZE, 8,
            0x3040, 4,
            EGL10.EGL_NONE
        };
        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfigs = new int[1];
        if (!mEgl.eglChooseConfig(mEglDisplay, configAttribs, configs, 1, numConfigs) || numConfigs[0] == 0) {
            int[] fallback = {
                EGL10.EGL_RED_SIZE, 5,
                EGL10.EGL_GREEN_SIZE, 6,
                EGL10.EGL_BLUE_SIZE, 5,
                EGL10.EGL_DEPTH_SIZE, 16,
                EGL10.EGL_NONE
            };
            mEgl.eglChooseConfig(mEglDisplay, fallback, configs, 1, numConfigs);
        }
        if (numConfigs[0] > 0) {
            mEglConfig = configs[0];
        }
        int[] ctxAttribs = {
            0x3098, 2,
            EGL10.EGL_NONE
        };
        mEglContext = mEgl.eglCreateContext(mEglDisplay, mEglConfig, EGL10.EGL_NO_CONTEXT, ctxAttribs);
    }
}

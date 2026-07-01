package com.trans;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

class GLSurfaceView$ComponentSizeChooser extends GLSurfaceView$BaseConfigChooser {
    protected int mAlphaSize;
    protected int mBlueSize;
    protected int mDepthSize;
    protected int mGreenSize;
    protected int mRedSize;
    protected int mStencilSize;
    private int[] mValue;
    GLSurfaceView this$0;

    public GLSurfaceView$ComponentSizeChooser(GLSurfaceView p0, int p1, int p2, int p3, int p4, int p5, int p6) {
        super(p0, null, 0);
        this.this$0 = p0;
        mRedSize = p1;
        mGreenSize = p2;
        mBlueSize = p3;
        mAlphaSize = p4;
        mDepthSize = p5;
        mStencilSize = p6;
        mValue = new int[1];
        mConfigSpec = new int[]{
            EGL10.EGL_RED_SIZE, p1,
            EGL10.EGL_GREEN_SIZE, p2,
            EGL10.EGL_BLUE_SIZE, p3,
            EGL10.EGL_ALPHA_SIZE, p4,
            EGL10.EGL_DEPTH_SIZE, p5,
            EGL10.EGL_STENCIL_SIZE, p6,
            EGL10.EGL_NONE
        };
    }

    private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue) {
        if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
            return mValue[0];
        }
        return defaultValue;
    }
}

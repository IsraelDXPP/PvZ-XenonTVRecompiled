package com.trans;

import android.util.Log;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

class GameView$ConfigChooser extends Object {
    private static int EGL_OPENGL_ES2_BIT = 4;
    private static int[] s_configAttribs2;
    protected int mAlphaSize;
    protected int mBlueSize;
    protected int mDepthSize;
    protected int mGreenSize;
    protected int mRedSize;
    protected int mStencilSize;
    private int[] mValue;

    public GameView$ConfigChooser(int p0, int p1, int p2, int p3, int p4, int p5) {
        mRedSize = p0;
        mGreenSize = p1;
        mBlueSize = p2;
        mAlphaSize = p3;
        mDepthSize = p4;
        mStencilSize = p5;
        mValue = new int[1];
    }

    private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue) {
        if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
            return mValue[0];
        }
        return defaultValue;
    }

    private void printConfig(EGL10 egl, EGLDisplay display, EGLConfig config) {
        int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
        int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
        int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
        int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);
        int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
        int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);
        Log.d("ConfigChooser", "RGBA: " + r + " " + g + " " + b + " " + a + " depth: " + d + " stencil: " + s);
    }

    private void printConfigs(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
        for (int i = 0; i < configs.length; i++) {
            printConfig(egl, display, configs[i]);
        }
    }

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
        int[] attribs = {
            EGL10.EGL_RED_SIZE, mRedSize,
            EGL10.EGL_GREEN_SIZE, mGreenSize,
            EGL10.EGL_BLUE_SIZE, mBlueSize,
            EGL10.EGL_ALPHA_SIZE, mAlphaSize,
            EGL10.EGL_DEPTH_SIZE, mDepthSize,
            EGL10.EGL_STENCIL_SIZE, mStencilSize,
            0x3040, EGL_OPENGL_ES2_BIT,
            EGL10.EGL_NONE
        };
        int[] numConfigs = new int[1];
        if (!egl.eglChooseConfig(display, attribs, null, 0, numConfigs) || numConfigs[0] <= 0) {
            return null;
        }
        EGLConfig[] configs = new EGLConfig[numConfigs[0]];
        if (!egl.eglChooseConfig(display, attribs, configs, numConfigs[0], numConfigs)) {
            return null;
        }
        return configs[0];
    }
}

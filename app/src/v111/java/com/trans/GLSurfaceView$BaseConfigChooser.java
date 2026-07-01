package com.trans;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;

abstract class GLSurfaceView$BaseConfigChooser extends GLSurfaceView$EGLConfigChooser {
    protected int[] mConfigSpec;
    GLSurfaceView this$0;

    public GLSurfaceView$BaseConfigChooser(GLSurfaceView p0, GLSurfaceView$EGLConfigChooser p1, int p2) {
        super();
        this.this$0 = p0;
    }

    private int[] filterConfigSpec(int[] p0) {
        if (p0 == null) return null;
        int len = p0.length;
        int[] result = new int[len + 2];
        System.arraycopy(p0, 0, result, 0, len - 1);
        result[len - 1] = 0x3038;
        result[len] = 0x3038;
        result[len + 1] = EGL10.EGL_NONE;
        return result;
    }

    public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
        int[] numConfigs = new int[1];
        if (!egl.eglChooseConfig(display, mConfigSpec, null, 0, numConfigs)) {
            return null;
        }
        int num = numConfigs[0];
        if (num <= 0) return null;
        EGLConfig[] configs = new EGLConfig[num];
        if (!egl.eglChooseConfig(display, mConfigSpec, configs, num, numConfigs)) {
            return null;
        }
        return configs[0];
    }
}

package com.trans;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

class GLSurfaceView$DefaultWindowSurfaceFactory extends Object {
    private GLSurfaceView$DefaultWindowSurfaceFactory() {
    }

    GLSurfaceView$DefaultWindowSurfaceFactory(int p0) {
    }

    public EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display, EGLConfig config, Object nativeWindow) {
        EGLSurface surface = null;
        try {
            surface = egl.eglCreateWindowSurface(display, config, nativeWindow, null);
        } catch (IllegalArgumentException e) {
        }
        return surface;
    }
}

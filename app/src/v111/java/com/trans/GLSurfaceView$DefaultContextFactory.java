package com.trans;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

class GLSurfaceView$DefaultContextFactory extends Object {
    private int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
    GLSurfaceView this$0;

    private GLSurfaceView$DefaultContextFactory(GLSurfaceView p0) {
        this.this$0 = p0;
    }

    GLSurfaceView$DefaultContextFactory(GLSurfaceView p0, int p1) {
        this.this$0 = p0;
        EGL_CONTEXT_CLIENT_VERSION = p1;
    }

    public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config) {
        int[] attribs = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE};
        return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attribs);
    }
}

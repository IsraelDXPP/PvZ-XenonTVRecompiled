package com.trans;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

class GameView$ContextFactory extends Object {
    private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

    private GameView$ContextFactory() {
    }

    GameView$ContextFactory(int p0) {
        EGL_CONTEXT_CLIENT_VERSION = p0;
    }

    public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config) {
        int[] attribs = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE};
        return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT, attribs);
    }
}

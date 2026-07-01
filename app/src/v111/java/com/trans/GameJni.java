package com.trans;

import android.os.Bundle;

import android.util.Log;

public class GameJni extends Object {
    private static boolean mLoaded;
    private static final String TAG = "GameJni";

    static {
        // Homura will be loaded on demand
    }

    public GameJni() {
    }

    public static native boolean filterKeyEvent(int p0, long p1, int p2, int p3, int p4, int p5, int p6);

    public static native void init(java.lang.String p0, java.lang.String p1, java.lang.String p2, com.trans.GameView p3, com.trans.GameActivity p4, android.os.Bundle p5, int p6, int p7);

    public static native boolean isInitialized();

    public static boolean isLoaded() {
        return mLoaded;
    }

    public static native void nativeDialogClick(int p0);

    public static native void onConfigurationChanged(int p0, int p1, java.lang.String p2);

    public static native boolean pause();

    public static native void queueAxisMovedEvent(int p0, int p1, long p2, int p3, float p4);

    public static native void queueDeviceEvent(int p0, boolean p1);

    public static native void queueKeyEvent(int p0, long p1, int p2, int p3, int p4, int p5, int p6);

    public static native void queuePointerEvent(int p0, int p1, long p2, int p3, int p4, int p5, float p6, float p7, float p8);

    public static native void queueSensorEvent(android.hardware.Sensor p0, int p1, int p2, long p3, float[] p4);

    public static native void readAudioData();

    public static native boolean render();

    public static native boolean resume();

    public static native void setFullVersion(boolean p0);

    public static native void setOfferFullOpenRes(boolean p0);

    public static native void setOfferRes(boolean p0);

    public static native void shutdown();

    public static native void surfaceCreated();

    public static native void textInput(java.lang.String p0);

    public static native void uninit();

    public static native void videoPlayCompleted();

    public static native void videoPlayError();

    public static native void windowFocusChanged(boolean p0);

    public static boolean load() {
        return load(null);
    }

    public static boolean load(java.lang.String[] paths) {
        Log.i(TAG, "load() called, mLoaded=" + mLoaded);
        if (isLoaded()) {
            Log.i(TAG, "already loaded");
            return true;
        }
        // Load GameLauncher FIRST via System.load so ART registers its JNI functions
        boolean launcherOk = loadLibrary(paths, "GameLauncher");
        Log.i(TAG, "GameLauncher loaded: " + launcherOk);
        if (!launcherOk) {
            return false;
        }
        // Then load Homura which sets up hooks
        boolean homuraOk = loadLibrary(paths, "Homura");
        Log.i(TAG, "Homura loaded: " + homuraOk);
        mLoaded = true;
        Log.i(TAG, "load() returning true");
        return true;
    }

    public static boolean loadLibrary(java.lang.String[] paths, java.lang.String libName) {
        String mappedName = java.lang.System.mapLibraryName(libName);
        Log.i(TAG, "loadLibrary(" + libName + "), mapped=" + mappedName);
        if (paths != null && paths.length > 0) {
            for (java.lang.String path : paths) {
                try {
                    java.lang.String absPath = new java.io.File(path, mappedName).getAbsolutePath();
                    Log.i(TAG, "  trying path " + absPath);
                    java.lang.System.load(absPath);
                    Log.i(TAG, "  success via path");
                    return true;
                } catch (java.lang.UnsatisfiedLinkError e) {
                    Log.i(TAG, "  path failed: " + e.getMessage());
                }
            }
        }
        try {
            Log.i(TAG, "  trying System.loadLibrary");
            java.lang.System.loadLibrary(libName);
            Log.i(TAG, "  success via System.loadLibrary");
            return true;
        } catch (java.lang.UnsatisfiedLinkError e) {
            Log.w(TAG, "  System.loadLibrary failed: " + e.getMessage());
            return false;
        }
    }
}

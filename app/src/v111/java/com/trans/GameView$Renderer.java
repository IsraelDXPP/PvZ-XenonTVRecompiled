package com.trans;

import android.content.Context;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.os.Bundle;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class GameView$Renderer extends Object implements GLSurfaceView$Renderer {
    public static int ACTION_HOVER_MOVE = 7;
    public static int ACTION_SCROLL = 8;
    public static int SOURCE_ANY = 256;
    public static int SOURCE_CLASS_BUTTON = 1;
    public static int SOURCE_CLASS_JOYSTICK = 16;
    public static int SOURCE_CLASS_MASK = 255;
    public static int SOURCE_CLASS_POINTER = 2;
    public static int SOURCE_CLASS_POSITION = 8;
    public static int SOURCE_CLASS_TRACKBALL = 4;
    public static int SOURCE_DPAD = 513;
    public static int SOURCE_GAMEPAD = 1025;
    public static int SOURCE_JOYSTICK = 16777232;
    public static int SOURCE_KEYBOARD = 257;
    public static int SOURCE_MOUSE = 8194;
    public static int SOURCE_STYLUS = 16386;
    public static int SOURCE_TOUCHPAD = 1048584;
    public static int SOURCE_TOUCHSCREEN = 4098;
    public static int SOURCE_TRACKBALL = 65540;
    static String TAG = "GameView$Renderer";
    GameActivity activity;
    Context ctx;
    boolean delayedShutdown;
    boolean rendering;
    boolean shutdown;

    public GameView$Renderer(Context p0, GameActivity p1) {
        ctx = p0;
        activity = p1;
        shutdown = false;
        rendering = false;
        delayedShutdown = false;
    }

    private boolean checkDataSpace() {
        return true;
    }

    private void handlePointerMotionEvent(MotionEvent event) {
        int action = event.getActionMasked();
        int pointerIndex = event.getActionIndex();
        int pointerId = event.getPointerId(pointerIndex);
        int deviceId = event.getDeviceId();
        long time = event.getEventTime();
        float x = event.getX(pointerIndex);
        float y = event.getY(pointerIndex);
        float pressure = event.getPressure(pointerIndex);
        int pointerCount = event.getPointerCount();
        int flag = (pointerIndex == pointerCount - 1 ? 1 : 0) | 2;
        GameJni.queuePointerEvent(pointerId, action, time, flag, deviceId, 1025, x, y, pressure);
    }

    public void handleConfigurationChanged(Configuration p0) {
        GameJni.onConfigurationChanged(p0.keyboard, p0.touchscreen, p0.locale.toString());
    }

    public void handleGenericMotionEvent(MotionEvent event) {
        int source = event.getSource();
        if ((source & InputDevice.SOURCE_CLASS_JOYSTICK) != 0) {
            handleJoyStickMotionEvent(event);
        } else if ((source & InputDevice.SOURCE_CLASS_POINTER) != 0) {
            handlePointerMotionEvent(event);
        }
    }

    private int lastDpadX, lastDpadY;

    public void handleJoyStickMotionEvent(MotionEvent event) {
        int deviceId = event.getDeviceId();
        long time = event.getEventTime();
        int historySize = event.getHistorySize();
        for (int i = 0; i < historySize; i++) {
            processJoystickMotion(event, deviceId, time, i, true);
            processDpadMotion(event, deviceId, time, i, true);
        }
        processJoystickMotion(event, deviceId, time, -1, false);
        processDpadMotion(event, deviceId, time, -1, false);
    }

    private void processDpadMotion(MotionEvent event, int deviceId, long time, int historyPos, boolean useHistory) {
        float hatX = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_HAT_X, historyPos) : event.getAxisValue(MotionEvent.AXIS_HAT_X);
        float hatY = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_HAT_Y, historyPos) : event.getAxisValue(MotionEvent.AXIS_HAT_Y);
        float x = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_X, historyPos) : event.getAxisValue(MotionEvent.AXIS_X);
        float y = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_Y, historyPos) : event.getAxisValue(MotionEvent.AXIS_Y);

        Log.d("Render", "axes: hatX=" + hatX + " hatY=" + hatY + " x=" + x + " y=" + y);

        int dpadX, dpadY;
        if (hatX != 0 || hatY != 0) {
            dpadX = hatX > 0.5f ? 1 : hatX < -0.5f ? -1 : 0;
            dpadY = hatY > 0.5f ? 1 : hatY < -0.5f ? -1 : 0;
        } else {
            dpadX = x > 0.5f ? 1 : x < -0.5f ? -1 : 0;
            dpadY = y > 0.5f ? 1 : y < -0.5f ? -1 : 0;
        }

        Log.d("Render", "dpad: X=" + dpadX + " Y=" + dpadY + " oldX=" + lastDpadX + " oldY=" + lastDpadY);

        int oldX = lastDpadX, oldY = lastDpadY;
        lastDpadX = dpadX;
        lastDpadY = dpadY;

        if (dpadX != oldX) {
            if (oldX != 0) GameJni.queueKeyEvent(0, time, deviceId, 1025, oldX < 0 ? KeyEvent.KEYCODE_DPAD_LEFT : KeyEvent.KEYCODE_DPAD_RIGHT, 0, 0);
            if (dpadX != 0) GameJni.queueKeyEvent(1, time, deviceId, 1025, dpadX < 0 ? KeyEvent.KEYCODE_DPAD_LEFT : KeyEvent.KEYCODE_DPAD_RIGHT, 0, 0);
        }
        if (dpadY != oldY) {
            if (oldY != 0) GameJni.queueKeyEvent(0, time, deviceId, 1025, oldY < 0 ? KeyEvent.KEYCODE_DPAD_UP : KeyEvent.KEYCODE_DPAD_DOWN, 0, 0);
            if (dpadY != 0) GameJni.queueKeyEvent(1, time, deviceId, 1025, dpadY < 0 ? KeyEvent.KEYCODE_DPAD_UP : KeyEvent.KEYCODE_DPAD_DOWN, 0, 0);
        }
    }

    private void processJoystickMotion(MotionEvent event, int deviceId, long time, int historyPos, boolean useHistory) {
        float x = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_X, historyPos) : event.getAxisValue(MotionEvent.AXIS_X);
        float y = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_Y, historyPos) : event.getAxisValue(MotionEvent.AXIS_Y);
        float z = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_Z, historyPos) : event.getAxisValue(MotionEvent.AXIS_Z);
        float rx = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_RX, historyPos) : event.getAxisValue(MotionEvent.AXIS_RX);
        float ry = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_RY, historyPos) : event.getAxisValue(MotionEvent.AXIS_RY);
        float lTrigger = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_LTRIGGER, historyPos) : event.getAxisValue(MotionEvent.AXIS_LTRIGGER);
        float rTrigger = useHistory ? event.getHistoricalAxisValue(MotionEvent.AXIS_RTRIGGER, historyPos) : event.getAxisValue(MotionEvent.AXIS_RTRIGGER);

        if (x != 0) GameJni.queueAxisMovedEvent(deviceId, MotionEvent.AXIS_X, time, 1, x);
        if (y != 0) GameJni.queueAxisMovedEvent(deviceId, MotionEvent.AXIS_Y, time, 1, y);
        if (z != 0) GameJni.queueAxisMovedEvent(deviceId, MotionEvent.AXIS_Z, time, 1, z);
        if (rx != 0) GameJni.queueAxisMovedEvent(deviceId, MotionEvent.AXIS_RX, time, 1, rx);
        if (ry != 0) GameJni.queueAxisMovedEvent(deviceId, MotionEvent.AXIS_RY, time, 1, ry);
        if (lTrigger != 0) GameJni.queueAxisMovedEvent(deviceId, MotionEvent.AXIS_LTRIGGER, time, 1, lTrigger);
        if (rTrigger != 0) GameJni.queueAxisMovedEvent(deviceId, MotionEvent.AXIS_RTRIGGER, time, 1, rTrigger);
    }

    public void handleKeyDown(int keyCode, KeyEvent event) {
        Log.d("Render", "handleKeyDown: keyCode=" + keyCode + " source=" + event.getSource() + " dev=" + event.getDeviceId());
        GameJni.queueKeyEvent(1, event.getEventTime(), event.getDeviceId(), 1025, keyCode, event.getUnicodeChar(), event.getRepeatCount());
    }

    public void handleKeyUp(int keyCode, KeyEvent event) {
        Log.d("Render", "handleKeyUp: keyCode=" + keyCode + " source=" + event.getSource() + " dev=" + event.getDeviceId());
        GameJni.queueKeyEvent(0, event.getEventTime(), event.getDeviceId(), 1025, keyCode, event.getUnicodeChar(), event.getRepeatCount());
    }

    public void handlePause() {
        GameJni.pause();
    }

    public void handleResume() {
        GameJni.resume();
    }

    public void handleSensorChanged(Sensor sensor, int type, int accuracy, long timestamp, float[] values) {
        GameJni.queueSensorEvent(sensor, type, accuracy, timestamp, values);
    }

    public void handleTextInput(String text) {
        GameJni.textInput(text);
    }

    public void handleTouch(MotionEvent event) {
        int action = event.getActionMasked();
        int pointerCount = event.getPointerCount();
        int deviceId = event.getDeviceId();
        long time = event.getEventTime();
        int actionIndex = event.getAction();

        for (int i = 0; i < pointerCount; i++) {
            int pointerId = event.getPointerId(i);
            float x = event.getX(i);
            float y = event.getY(i);
            float pressure = event.getPressure(i);
            int flag = (i == pointerCount - 1 ? 1 : 0);
            if (i == (actionIndex & 0xff)) flag |= 2;
            GameJni.queuePointerEvent(pointerId, action, time, flag, deviceId, 1025, x, y, pressure);
        }
    }

    public void onDrawFrame(GL10 p0) {
        GameJni.render();
    }

    public void onSurfaceChanged(GL10 p0, int p1, int p2) {
        Log.i(TAG, "onSurfaceChanged: " + p1 + "x" + p2);
        String sourceDir = ctx.getApplicationInfo().sourceDir;
        String dataDir = ctx.getApplicationInfo().dataDir;
        String filesDir = ctx.getExternalFilesDir(null).getAbsolutePath();
        Bundle metaData = null;
        try {
            metaData = activity.getPackageManager()
                .getActivityInfo(activity.getComponentName(), 128)
                .metaData;
        } catch (android.content.pm.PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        GameJni.init(sourceDir, dataDir, filesDir, activity.getView(), activity, metaData, p1, p2);
        Log.i(TAG, "onSurfaceChanged: GameJni.init() done");
        rendering = true;
    }

    public void onSurfaceCreated(GL10 p0, EGLConfig p1) {
        Log.i(TAG, "onSurfaceCreated");
        GameJni.surfaceCreated();
        Log.i(TAG, "onSurfaceCreated: GameJni.surfaceCreated() done");
    }
}

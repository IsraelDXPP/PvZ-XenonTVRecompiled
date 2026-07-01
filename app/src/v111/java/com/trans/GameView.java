package com.trans;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Configuration;
import android.hardware.SensorEvent;
import android.os.Handler;
import android.text.Spanned;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

public class GameView extends GLSurfaceView {
    private static boolean DEBUG;
    protected static int HIDE_GAME = 1;
    protected static int HIDE_KEYBOARD = 2;
    protected static int KEYBOARD_EMAIL = 3;
    protected static int KEYBOARD_NORMAL = 1;
    protected static int KEYBOARD_PASSWORD = 2;
    protected static int KEYBOARD_URL = 4;
    protected static int KEYBOARD_USERNAME = 5;
    protected static int SHOW_GAME;
    protected static int SHOW_KEYBOARD = 1;
    private static String TAG = "GameView";
    private GameActivity gameActivity;
    private int heightAs;
    Handler mHandler;
    private AlertDialog mTextInputDialog;
    private EditText mTextInputWidget;
    private GameView$Renderer renderer;
    private boolean shiLiuBiJiu;
    private int widthAs;

    public GameView(Context p0, GameActivity p1) {
        super(p0);
        gameActivity = p1;
        renderer = new GameView$Renderer(p0, p1);
        setRenderer(renderer);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
    }

    public GameView(Context p0, GameActivity p1, boolean p2, int p3, int p4) {
        super(p0);
        gameActivity = p1;
        shiLiuBiJiu = p2;
        widthAs = p3;
        heightAs = p4;
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
    }

    public static void checkEglError(String p0, EGL10 p1) {
        if (p1 == null) return;
        int error = p1.eglGetError();
        if (error != EGL10.EGL_SUCCESS) {
            Log.e(TAG, p0 + ": EGL error 0x" + Integer.toHexString(error));
        }
    }

    private boolean filterKeyEvent(boolean down, KeyEvent event) {
        return GameJni.filterKeyEvent(down ? 1 : 0, event.getEventTime(), event.getKeyCode(), event.getScanCode(), event.getMetaState(), event.getRepeatCount(), event.getDeviceId());
    }

    private void init(Context p0, boolean p1, int p2, int p3) {
    }

    public void _hideKeyboard(boolean p0) {
        hideKeyboard();
    }

    public void _show(boolean p0) {
        show(p0);
    }

    public void _showKeyboard(int p0, String p1, String p2, String p3) {
        showKeyboard(p0, p1, p2, p3);
    }

    public void hideKeyboard() {
        InputMethodManager imm = (InputMethodManager) getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        if (imm != null) {
            imm.hideSoftInputFromWindow(getWindowToken(), 0);
        }
    }

    public void onConfigurationChanged(Configuration p0) {
        if (renderer != null) {
            renderer.handleConfigurationChanged(p0);
        }
    }

    public boolean onGenericMotionEvent(MotionEvent event) {
        MotionEvent copy = MotionEvent.obtain(event);
        queueEvent(new Runnable() {
            public void run() {
                if (renderer != null) {
                    renderer.handleGenericMotionEvent(copy);
                }
                copy.recycle();
            }
        });
        return true;
    }

    private static int mapControllerKey(int keyCode) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_BUTTON_A: return KeyEvent.KEYCODE_DPAD_CENTER;
            case KeyEvent.KEYCODE_BUTTON_B: return KeyEvent.KEYCODE_BACK;
            case KeyEvent.KEYCODE_BUTTON_X: return KeyEvent.KEYCODE_DPAD_CENTER;
            case KeyEvent.KEYCODE_BUTTON_Y: return KeyEvent.KEYCODE_BACK;
            case KeyEvent.KEYCODE_BUTTON_START: return KeyEvent.KEYCODE_DPAD_CENTER;
            case KeyEvent.KEYCODE_BUTTON_SELECT: return KeyEvent.KEYCODE_BACK;
            case KeyEvent.KEYCODE_BUTTON_L1: return KeyEvent.KEYCODE_Q;
            case KeyEvent.KEYCODE_BUTTON_R1: return KeyEvent.KEYCODE_E;
            default: return keyCode;
        }
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_VOLUME_UP || keyCode == KeyEvent.KEYCODE_VOLUME_DOWN) {
            return false;
        }
        final int code = mapControllerKey(keyCode);
        final KeyEvent copy = new KeyEvent(event);
        queueEvent(new Runnable() {
            public void run() {
                if (renderer != null) {
                    renderer.handleKeyDown(code, copy);
                }
            }
        });
        return true;
    }

    public boolean onKeyUp(int keyCode, KeyEvent event) {
        final int code = mapControllerKey(keyCode);
        final KeyEvent copy = new KeyEvent(event);
        queueEvent(new Runnable() {
            public void run() {
                if (renderer != null) {
                    renderer.handleKeyUp(code, copy);
                }
            }
        });
        return true;
    }

    protected void onMeasure(int p0, int p1) {
        setMeasuredDimension(getDefaultSize(0, p0), getDefaultSize(0, p1));
    }

    public void onPause() {
        super.onPause();
    }

    public void onResume() {
        super.onResume();
    }

    public void onSensorChanged(SensorEvent event) {
        if (renderer != null) {
            renderer.handleSensorChanged(event.sensor, event.sensor.getType(), event.accuracy, event.timestamp, event.values);
        }
    }

    public boolean onTouchEvent(MotionEvent event) {
        final MotionEvent copy = MotionEvent.obtain(event);
        queueEvent(new Runnable() {
            public void run() {
                if (renderer != null) {
                    renderer.handleTouch(copy);
                }
                copy.recycle();
            }
        });
        return true;
    }

    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        GameJni.windowFocusChanged(hasFocus);
    }

    public void show(boolean p0) {
        setVisibility(p0 ? View.VISIBLE : View.GONE);
    }

    public void showKeyboard(int type, String hint, String text, String callback) {
        AlertDialog.Builder builder = new AlertDialog.Builder(getContext());
        final EditText edit = new EditText(getContext());
        if (hint != null) edit.setHint(hint);
        if (text != null) edit.setText(text);
        builder.setView(edit);
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                String input = edit.getText().toString();
                if (renderer != null) {
                    renderer.handleTextInput(input);
                }
                GameJni.textInput(input);
            }
        });
        builder.setCancelable(false);
        mTextInputDialog = builder.show();
    }

    public void setSwapMode(int mode) {
        if (mGLThread != null) {
            mGLThread.setSwapMode(mode);
        }
    }

    public void stopGame() {
    }

    public void swapBuffers() {
        super.swapBuffers();
    }

    public void textInput(String p0) {
        if (renderer != null) {
            renderer.handleTextInput(p0);
        }
    }

    public void update() {
        super.handleEvents();
    }
}

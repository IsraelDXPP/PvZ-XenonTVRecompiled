package com.trans;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Intent;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Bundle;
import android.os.FileObserver;
import android.os.Handler;
import android.provider.Settings;
import android.util.Log;
import android.view.KeyEvent;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.widget.FrameLayout;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Locale;

public class GameActivity extends Activity {
    protected static int ATTACH_VIDEO_PLAYER = 257;
    protected static int DETACH_VIDEO_PLAYER = 258;
    public static String TAG = "GameActivity";
    private static GameActivity mCurActivity;
    private FileObserver fileObserver;
    private boolean isFileObserverLaunched;
    Handler mHandler;
    protected FrameLayout mLayout;
    protected GameVideo mVideo;
    protected GameView mView;

    public GameActivity() {
        mLayout = null;
        mView = null;
        mVideo = null;
        mHandler = new Handler();
        isFileObserverLaunched = false;
    }

    private boolean CheckLenovoLicense() {
        return false;
    }

    public static void copyDir(File src, File dst) {
        if (src == null || dst == null) return;
        if (src.isDirectory()) {
            if (!dst.exists()) {
                dst.mkdirs();
            }
            String[] children = src.list();
            if (children != null) {
                for (String child : children) {
                    copyDir(new File(src, child), new File(dst, child));
                }
            }
        } else if (src.isFile()) {
            try {
                InputStream in = new FileInputStream(src);
                OutputStream out = new FileOutputStream(dst);
                byte[] buf = new byte[8192];
                int len;
                while ((len = in.read(buf)) > 0) {
                    out.write(buf, 0, len);
                }
                in.close();
                out.close();
            } catch (IOException e) {
                Log.e(TAG, "copyDir: failed to copy " + src + " to " + dst);
            }
        }
    }

    public static void deleteRecursive(File file) {
        if (file == null) return;
        if (file.isDirectory()) {
            File[] children = file.listFiles();
            if (children != null) {
                for (File child : children) {
                    deleteRecursive(child);
                }
            }
        }
        file.delete();
    }

    public static GameActivity get() {
        return mCurActivity;
    }

    public String GetDataValue(String p0) {
        return null;
    }

    public void _attachVideoPlayer() {
        if (mVideo != null) {
            mVideo._show(true);
        }
    }

    public void _detachVideoPlayer() {
        if (mVideo != null) {
            mVideo._show(false);
        }
    }

    public void _goToMarket() {
    }

    public void _hideAD() {
    }

    public boolean _isDisplayCursor() {
        return false;
    }

    public void _postData(String p0, String p1) {
    }

    public void _postXMLFileData(String p0) {
    }

    public boolean _setRcDefaultMode() {
        return true;
    }

    public boolean _setRcSensorAirMouse() {
        return false;
    }

    public void _show91MoreGame() {
    }

    public void _showAD(int p0, int p1) {
    }

    public void _showAlipay() {
    }

    public void _showAppNotProperlyInstalledDialog() {
    }

    public boolean _showCursor(boolean p0) {
        return false;
    }

    public void _showDataFullDialog() {
    }

    public void _showMessageBox(String p0, String p1, int p2, int p3) {
        Log.i(TAG, "_showMessageBox: " + p0 + " / " + p1);
    }

    public void _showMoreGame() {
    }

    public void _showNdpay() {
    }

    public void _showOffer(String p0, int p1, int p2, int p3, int p4, int p5) {
    }

    public void appNotProperlyInstalled() {
        _showAppNotProperlyInstalledDialog();
    }

    public void attachVideoPlayer() {
        _attachVideoPlayer();
    }

    public void checkAndDeleteOldBackups() {
    }

    public void detachVideoPlayer() {
        _detachVideoPlayer();
    }

    public void fullscreen() {
        if (Build.VERSION.SDK_INT >= 30) {
            getWindow().setDecorFitsSystemWindows(false);
            WindowInsetsController controller = getWindow().getInsetsController();
            if (controller != null) {
                controller.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
                controller.setSystemBarsBehavior(WindowInsetsController.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);
            }
        } else if (Build.VERSION.SDK_INT >= 19) {
            getWindow().getDecorView().setSystemUiVisibility(
                6 | 2 | 256 | 512
            );
        }
    }

    public String getAndroidId() {
        return Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
    }

    public String getDeviceId() {
        return "";
    }

    public String getExternalStorageDirectory() {
        File extDir = getExternalFilesDir(null);
        return extDir != null ? extDir.getAbsolutePath() : getFilesDir().getAbsolutePath();
    }

    public int getKeyboard() {
        Configuration config = getResources().getConfiguration();
        return config.keyboard;
    }

    public String getLocale() {
        return Locale.getDefault().toString();
    }

    public String getModel() {
        return Build.MODEL;
    }

    public int getSdkVersion() {
        return Build.VERSION.SDK_INT;
    }

    public float getSensorMaxRange(int type) {
        SensorManager sm = (SensorManager) getSystemService(SENSOR_SERVICE);
        if (sm == null) return 0;
        Sensor s = sm.getDefaultSensor(type);
        return s != null ? s.getMaximumRange() : 0;
    }

    public float getSensorResolution(int type) {
        SensorManager sm = (SensorManager) getSystemService(SENSOR_SERVICE);
        if (sm == null) return 0;
        Sensor s = sm.getDefaultSensor(type);
        return s != null ? s.getResolution() : 0;
    }

    public int getTouchScreen() {
        Configuration config = getResources().getConfiguration();
        return config.touchscreen;
    }

    public GameVideo getVideoPlayer() {
        return mVideo;
    }

    public GameView getView() {
        return mView;
    }

    public void goToMarket() {
    }

    public boolean hasSensor(int type) {
        SensorManager sm = (SensorManager) getSystemService(SENSOR_SERVICE);
        if (sm == null) return false;
        return sm.getDefaultSensor(type) != null;
    }

    public void hideAD() {
    }

    public void installPackage(String p0) {
    }

    public void onConfigurationChanged(Configuration p0) {
        super.onConfigurationChanged(p0);
        if (mView != null) {
            mView.onConfigurationChanged(p0);
        }
    }

    protected void onCreate(Bundle p0) {
        requestWindowFeature(1);
        if (Build.VERSION.SDK_INT >= 14) {
            setTheme(android.R.style.Theme_DeviceDefault_NoActionBar);
        }
        getWindow().setSoftInputMode(16);
        if (Build.VERSION.SDK_INT >= 16) {
            getWindow().getDecorView().setSystemUiVisibility(1028);
        }
        if (Build.VERSION.SDK_INT >= 29) {
            getWindow().setNavigationBarContrastEnforced(false);
        }
        if (Build.VERSION.SDK_INT >= 21) {
            getWindow().setStatusBarColor(0);
            getWindow().setNavigationBarColor(0);
        }
        if (Build.VERSION.SDK_INT >= 28) {
            getWindow().getAttributes().layoutInDisplayCutoutMode = 1;
        } else {
            getWindow().setFlags(1024, 1024);
        }
        if (Build.VERSION.SDK_INT >= 30) {
            getWindow().setDecorFitsSystemWindows(false);
            WindowInsetsController controller = getWindow().getInsetsController();
            if (controller != null) {
                controller.hide(WindowInsets.Type.statusBars() | WindowInsets.Type.displayCutout());
                controller.setSystemBarsBehavior(2);
            }
        }
        getWindow().addFlags(128);
        super.onCreate(p0);

        android.content.SharedPreferences prefs = getSharedPreferences("data", 0);
        isFileObserverLaunched = prefs.getBoolean("autoBackUp", true);
        if (isFileObserverLaunched) {
            checkAndDeleteOldBackups();
            File userdataDir = new File(getExternalFilesDir(null), "userdata");
            fileObserver = new FileObserver(userdataDir.getAbsolutePath(), 8) {
                public void onEvent(int event, String path) {
                }
            };
            fileObserver.startWatching();
        }

        boolean first = prefs.getBoolean("first", true);
        if (first) {
            startActivity(new Intent(this, UnzipActivity.class));
            finish();
            return;
        }

        String dataPath = getApplicationInfo().dataDir + "/lib";
        String sysPath = "/system/lib/" + getPackageName();
        String[] searchPaths = new String[]{dataPath, sysPath};
        Log.i(TAG, "onCreate: calling GameJni.load with paths: " + dataPath + ", " + sysPath);
        if (!GameJni.load(searchPaths)) {
            Log.w(TAG, "onCreate: GameJni.load failed, finishing");
            finish();
            return;
        }
        Log.i(TAG, "onCreate: GameJni.load succeeded, calling setup()");
        setup();
    }

    protected void onDestroy() {
        super.onDestroy();
    }

    protected void onPause() {
        super.onPause();
        if (mView != null) mView.onPause();
    }

    protected void onResume() {
        super.onResume();
        if (mView != null) mView.onResume();
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (mView != null) {
            return mView.dispatchKeyEvent(event);
        }
        return super.onKeyDown(keyCode, event);
    }

    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (mView != null) {
            return mView.dispatchKeyEvent(event);
        }
        return super.onKeyUp(keyCode, event);
    }

    protected void onStart() {
        super.onStart();
    }

    protected void onStop() {
        super.onStop();
    }

    public void postData(String p0, String p1) {
    }

    public void postXMLFileData(String p0) {
    }

    protected void registerLenovoBroadcastReceiver() {
    }

    public void restoreCursor() {
    }

    public void setOfferFullOpenRes(boolean p0) {
        GameJni.setOfferFullOpenRes(p0);
    }

    public void setOfferRes(boolean p0) {
        GameJni.setOfferRes(p0);
    }

    public void setup() {
        boolean init = GameJni.isInitialized();
        Log.i(TAG, "setup: GameJni.isInitialized() = " + init);
        if (init) {
            Log.w(TAG, "setup: already initialized, finishing");
            finish();
            return;
        }
        mCurActivity = this;
        startGame();
        mVideo = new GameVideo(this);
    }

    public void setupView() {
        startGame();
    }

    public void show91MoreGame() {
    }

    public void showAD(int p0, int p1) {
    }

    public void showAlipay() {
    }

    public boolean showCursor(boolean p0) {
        return false;
    }

    public void showDataFullDialog() {
    }

    public void showMessageBox(String p0, String p1, int p2, int p3) {
        _showMessageBox(p0, p1, p2, p3);
    }

    public void showMoreGame() {
    }

    public void showNdpay() {
    }

    public void showOffer(String p0, int p1, int p2, int p3, int p4, int p5) {
    }

    public void shutdown() {
        GameJni.shutdown();
    }

    public void shutdown(boolean p0) {
        GameJni.shutdown();
    }

    public void startGame() {
        Log.i(TAG, "startGame: creating view");
        ACPManager.getInstance().setAssets(getAssets(), "files/");
        mView = new GameView(this, this);
        if (mLayout == null) {
            mLayout = new FrameLayout(this);
        }
        mLayout.setBackgroundColor(0xFF000000);
        mLayout.addView(mView, new FrameLayout.LayoutParams(-1, -1));
        setContentView(mLayout);
        new Thread(new Runnable() {
            public void run() {
            }
        }).start();
    }

    public boolean startSensor(int type) {
        return false;
    }

    public boolean stopSensor(int type) {
        return false;
    }
}

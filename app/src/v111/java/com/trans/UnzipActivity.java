package com.trans;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.TextView;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class UnzipActivity extends Activity {
    public static String TAG = "UnzipActivity";
    private Handler handler;
    private TextView progressTextView;

    public UnzipActivity() {
    }

    private int unzipAsset(String zipName, File targetDir) throws IOException {
        AssetManager am = getAssets();
        InputStream is = am.open(zipName);
        ZipInputStream zis = new ZipInputStream(is);
        byte[] buffer = new byte[65536];
        int count = 0;
        long lastUpdate = 0;

        ZipEntry entry;
        while ((entry = zis.getNextEntry()) != null) {
            count++;
            String name = entry.getName();
            File outFile = new File(targetDir, name);

            if (entry.isDirectory()) {
                outFile.mkdirs();
            } else {
                outFile.getParentFile().mkdirs();
                BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(outFile), 65536);
                int len;
                while ((len = zis.read(buffer)) > 0) {
                    bos.write(buffer, 0, len);
                }
                bos.close();
            }
            zis.closeEntry();

            long now = System.currentTimeMillis();
            if (now - lastUpdate > 100) {
                lastUpdate = now;
                final int c = count;
                handler.post(new Runnable() {
                    public void run() {
                        progressTextView.setText("Extracting: " + c + " files...");
                    }
                });
            }
        }
        zis.close();
        is.close();
        return count;
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        FrameLayout layout = new FrameLayout(this);

        progressTextView = new TextView(this);
        progressTextView.setText("Extracting files...");
        progressTextView.setTextSize(18);
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.WRAP_CONTENT,
            FrameLayout.LayoutParams.WRAP_CONTENT
        );
        lp.gravity = Gravity.CENTER;
        layout.addView(progressTextView, lp);

        setContentView(layout);

        handler = new Handler();

        new Thread(new Runnable() {
            public void run() {
                try {
                    File filesDir = getExternalFilesDir(null);
                    final int fileCount = unzipAsset("files.zip", filesDir);
                    unzipAsset("userdata.zip", filesDir);
                    getSharedPreferences("data", 0).edit().putBoolean("first", false).apply();
                    handler.post(new Runnable() {
                        public void run() {
                            progressTextView.setText("Done! (" + fileCount + " files)");
                        }
                    });
                    try { Thread.sleep(300); } catch (InterruptedException e) {}
                } catch (Exception e) {
                    Log.e(TAG, "Error extracting: " + e.getMessage());
                    getSharedPreferences("data", 0).edit().putBoolean("first", false).apply();
                }

                handler.post(new Runnable() {
                    public void run() {
                        Intent intent = new Intent(UnzipActivity.this, com.trans.pvz.GameLauncher.class);
                        startActivity(intent);
                        finish();
                    }
                });
            }
        }).start();
    }
}

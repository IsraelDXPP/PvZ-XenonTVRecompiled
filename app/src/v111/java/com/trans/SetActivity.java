package com.trans;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.util.Pair;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.LinearLayout;
import android.widget.NumberPicker;
import android.widget.TextView;
import android.widget.ScrollView;
import android.widget.Button;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

public class SetActivity extends Activity {
    public static String TAG = "SetActivity";

    public SetActivity() {
    }

    public static void addFolderToZip(File folder, ZipOutputStream zos) {
        if (folder == null || zos == null) return;
        File[] files = folder.listFiles();
        if (files == null) return;
        byte[] buf = new byte[8192];
        try {
            for (File f : files) {
                if (f.isDirectory()) {
                    addFolderToZip(f, zos);
                } else if (f.isFile()) {
                    ZipEntry entry = new ZipEntry(f.getName());
                    zos.putNextEntry(entry);
                    FileInputStream fis = new FileInputStream(f);
                    int len;
                    while ((len = fis.read(buf)) > 0) {
                        zos.write(buf, 0, len);
                    }
                    fis.close();
                    zos.closeEntry();
                }
            }
        } catch (IOException e) {
            Log.e(TAG, "addFolderToZip: " + e.getMessage());
        }
    }

    public static void copyDir(File src, File dst) {
        GameActivity.copyDir(src, dst);
    }

    public static void deleteRecursive(File file) {
        GameActivity.deleteRecursive(file);
    }

    public static List findNumDirs(File dir) {
        List<File> result = new ArrayList<>();
        if (dir == null || !dir.isDirectory()) return result;
        File[] files = dir.listFiles();
        if (files == null) return result;
        for (File f : files) {
            if (f.isDirectory()) {
                String name = f.getName();
                try {
                    Integer.parseInt(name);
                    result.add(f);
                } catch (NumberFormatException e) {
                }
            }
        }
        return result;
    }

    public void onActivityResult(int p0, int p1, Intent p2) {
        super.onActivityResult(p0, p1, p2);
    }

    public void onBackPressed() {
        super.onBackPressed();
    }

    protected void onCreate(Bundle p0) {
        super.onCreate(p0);
        ScrollView scroll = new ScrollView(this);
        LinearLayout layout = new LinearLayout(this);
        layout.setOrientation(LinearLayout.VERTICAL);
        layout.setPadding(16, 16, 16, 16);

        TextView title = new TextView(this);
        title.setText("Settings");
        title.setTextSize(24);
        layout.addView(title);

        Button backButton = new Button(this);
        backButton.setText("Back to Game");
        backButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finish();
            }
        });
        layout.addView(backButton);

        scroll.addView(layout);
        setContentView(scroll);
    }

    public boolean onCreateOptionsMenu(Menu p0) {
        return true;
    }

    public boolean onMenuItemSelected(int p0, MenuItem p1) {
        return false;
    }
}

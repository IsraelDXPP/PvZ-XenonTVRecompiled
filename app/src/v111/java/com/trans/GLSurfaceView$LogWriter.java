package com.trans;

import java.io.Writer;
import android.util.Log;

class GLSurfaceView$LogWriter extends Writer {
    private StringBuilder mBuilder = new StringBuilder();
    private String TAG = "GLSurfaceView";

    GLSurfaceView$LogWriter() {
    }

    private void flushBuilder() {
        if (mBuilder.length() > 0) {
            Log.v(TAG, mBuilder.toString());
            mBuilder.delete(0, mBuilder.length());
        }
    }

    public void write(char[] buf, int offset, int count) {
        for (int i = 0; i < count; i++) {
            char c = buf[offset + i];
            if (c == '\n') {
                flushBuilder();
            } else {
                mBuilder.append(c);
            }
        }
    }

    public void close() {
        flushBuilder();
    }

    public void flush() {
        flushBuilder();
    }
}

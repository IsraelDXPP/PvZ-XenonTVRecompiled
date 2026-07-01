package com.trans;

import android.media.AudioTrack;

import java.lang.reflect.Method;

class GameAudioTrack extends AudioTrack {
    private final int mFrameSize;

    public GameAudioTrack(int i, int i2, int i3, int i4, int i5, int i6) throws IllegalArgumentException {
        super(i, i2, i3, i4, i5, i6);
        if (i4 == 2) {
            this.mFrameSize = getChannelCount() * 2;
        } else {
            this.mFrameSize = getChannelCount();
        }
    }

    @Override
    public void play() throws IllegalStateException {
        super.play();
        initBuffer();
    }

    public void initBuffer() {
        int nativeFrameCount = getFrameCount() * this.mFrameSize;
        write(new byte[nativeFrameCount], 0, nativeFrameCount);
    }

    /**
     * Original code called the hidden AudioTrack.getNativeFrameCount().
     * That method is no longer in the public SDK (compileSdk 33), so we try it
     * via reflection first and fall back to the public getBufferSizeInFrames()
     * (API 23+), which returns the same buffer frame count.
     */
    private int getFrameCount() {
        try {
            Method m = AudioTrack.class.getMethod("getNativeFrameCount");
            Object r = m.invoke(this);
            if (r instanceof Integer) {
                return (Integer) r;
            }
        } catch (Throwable ignored) {
        }
        return getBufferSizeInFrames();
    }
}

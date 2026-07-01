package com.trans;

import android.media.AudioTrack;
import java.nio.ByteBuffer;

public class GameAudio {
    private static final GameAudio mInst = new GameAudio();
    byte[] mAudioData = new byte[8192];
    private GameAudioTrack mAudioTrack;

    public static GameAudio getInstance() {
        return mInst;
    }

    public boolean initAudio(int i, int i2, int i3) {
        if (this.mAudioTrack != null || i != 44100 || i2 != 2 || i3 != 16) {
            return false;
        }
        GameAudioTrack gameAudioTrack = new GameAudioTrack(3, i, 3, 2, AudioTrack.getMinBufferSize(i, 3, 2), 1);
        this.mAudioTrack = gameAudioTrack;
        int state = gameAudioTrack.getState();
        GameAudioTrack gameAudioTrack2 = this.mAudioTrack;
        if (state != 1) {
            gameAudioTrack2.release();
            this.mAudioTrack = null;
            return false;
        }
        gameAudioTrack2.setPlaybackPositionUpdateListener(new AudioTrack.OnPlaybackPositionUpdateListener() {
            @Override
            public void onMarkerReached(AudioTrack audioTrack) {
            }

            @Override
            public void onPeriodicNotification(AudioTrack audioTrack) {
            }
        });
        this.mAudioTrack.play();
        return true;
    }

    public void uninitAudio() {
        GameAudioTrack gameAudioTrack = this.mAudioTrack;
        if (gameAudioTrack != null) {
            gameAudioTrack.stop();
            this.mAudioTrack.release();
            this.mAudioTrack = null;
        }
    }

    public void writeData(ByteBuffer byteBuffer, int i, int i2) {
        if (this.mAudioTrack != null) {
            byteBuffer.position(i);
            byteBuffer.get(this.mAudioData, 0, i2);
            this.mAudioTrack.write(this.mAudioData, 0, i2);
        }
    }

    public void pause() {
        GameAudioTrack gameAudioTrack = this.mAudioTrack;
        if (gameAudioTrack == null || gameAudioTrack.getPlayState() != 3) {
            return;
        }
        this.mAudioTrack.pause();
    }

    public void resume() {
        GameAudioTrack gameAudioTrack = this.mAudioTrack;
        if (gameAudioTrack == null || gameAudioTrack.getPlayState() == 3) {
            return;
        }
        this.mAudioTrack.play();
    }
}

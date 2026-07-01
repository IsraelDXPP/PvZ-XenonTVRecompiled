package com.trans;

import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Handler;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.FrameLayout;
import java.io.IOException;

public class GameVideo extends Object {
    private static int HIDE_VIDEO;
    private static int SHOW_VIDEO;
    private static String TAG = "GameVideo";
    private int heightAs;
    private GameActivity mActivity;
    private boolean mAttached;
    Handler mHandler;
    private boolean mHasSurface;
    private SurfaceHolder mHolder;
    private MediaPlayer mMediaPlayer;
    private String mPath;
    private boolean mPreparePending;
    private boolean mPrepared;
    private boolean mStartPending;
    private SurfaceView mView;
    private boolean mVisible;
    private boolean shiLiuBiJiu;
    private int widthAs;

    public GameVideo(GameActivity p0) {
        mActivity = p0;
        mHandler = new Handler();
        mView = new GameVideo$MySurfaceView(this, p0);
        mView.getHolder().addCallback(new SurfaceHolder.Callback() {
            public void surfaceCreated(SurfaceHolder holder) {
                mHasSurface = true;
                mHolder = holder;
                if (mPreparePending) {
                    open(mPath);
                    mPreparePending = false;
                }
            }

            public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
            }

            public void surfaceDestroyed(SurfaceHolder holder) {
                mHasSurface = false;
                mHolder = null;
            }
        });
    }

    private void attach() {
        if (!mAttached && mView != null) {
            FrameLayout layout = mActivity.mLayout;
            if (layout != null) {
                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(
                    FrameLayout.LayoutParams.MATCH_PARENT,
                    FrameLayout.LayoutParams.MATCH_PARENT
                );
                layout.addView(mView, lp);
                mView.setVisibility(View.GONE);
                mAttached = true;
            }
        }
    }

    private void detach() {
        if (mAttached && mView != null) {
            FrameLayout layout = mActivity.mLayout;
            if (layout != null) {
                layout.removeView(mView);
                mAttached = false;
            }
        }
    }

    private boolean init() {
        return true;
    }

    public void _show(boolean p0) {
        show(p0);
    }

    public boolean close() {
        if (mMediaPlayer != null) {
            try {
                mMediaPlayer.stop();
                mMediaPlayer.release();
            } catch (Exception e) {
            }
            mMediaPlayer = null;
        }
        mPrepared = false;
        mPath = null;
        return true;
    }

    public void destroy() {
        close();
    }

    public SurfaceView getView() {
        return mView;
    }

    public boolean isPlaying() {
        return mMediaPlayer != null && mMediaPlayer.isPlaying();
    }

    public void onBufferingUpdate(MediaPlayer p0, int p1) {
    }

    public void onCompletion(MediaPlayer mp) {
        Log.i(TAG, "onCompletion");
        GameJni.videoPlayCompleted();
    }

    public boolean onError(MediaPlayer mp, int what, int extra) {
        Log.e(TAG, "onError: what=" + what + " extra=" + extra);
        GameJni.videoPlayError();
        return true;
    }

    public void onPause() {
        if (mMediaPlayer != null && mMediaPlayer.isPlaying()) {
            mMediaPlayer.pause();
        }
    }

    public void onPrepared(MediaPlayer mp) {
        Log.i(TAG, "onPrepared");
        mPrepared = true;
        if (mStartPending) {
            mp.start();
            mStartPending = false;
        }
    }

    public void onResume() {
    }

    public boolean open(String path) {
        Log.i(TAG, "open: " + path);
        mPath = path;

        if (!mHasSurface) {
            mPreparePending = true;
            return true;
        }

        if (mMediaPlayer != null) {
            mMediaPlayer.release();
            mMediaPlayer = null;
        }

        try {
            mMediaPlayer = new MediaPlayer();
            mMediaPlayer.setDataSource(path);
            mMediaPlayer.setDisplay(mHolder);
            mMediaPlayer.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                public void onPrepared(MediaPlayer mp) {
                    GameVideo.this.onPrepared(mp);
                }
            });
            mMediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                public void onCompletion(MediaPlayer mp) {
                    GameVideo.this.onCompletion(mp);
                }
            });
            mMediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
                public boolean onError(MediaPlayer mp, int what, int extra) {
                    return GameVideo.this.onError(mp, what, extra);
                }
            });
            mMediaPlayer.setOnBufferingUpdateListener(new MediaPlayer.OnBufferingUpdateListener() {
                public void onBufferingUpdate(MediaPlayer mp, int percent) {
                    GameVideo.this.onBufferingUpdate(mp, percent);
                }
            });
            mMediaPlayer.prepareAsync();
            return true;
        } catch (IOException e) {
            Log.e(TAG, "open failed: " + e.getMessage());
            return false;
        }
    }

    public boolean pause() {
        if (mMediaPlayer != null && mMediaPlayer.isPlaying()) {
            mMediaPlayer.pause();
            return true;
        }
        return false;
    }

    public boolean play() {
        if (mMediaPlayer != null) {
            if (mPrepared) {
                mMediaPlayer.start();
                return true;
            } else {
                mStartPending = true;
                return true;
            }
        }
        return false;
    }

    public boolean resume() {
        return play();
    }

    public void show(boolean p0) {
        if (mView != null) {
            mView.setVisibility(p0 ? View.VISIBLE : View.GONE);
            if (p0) {
                mView.bringToFront();
            }
        }
    }

    public boolean stop() {
        if (mMediaPlayer != null) {
            try {
                mMediaPlayer.stop();
                mMediaPlayer.prepareAsync();
            } catch (Exception e) {
            }
            return true;
        }
        return false;
    }

    public void surfaceChanged(SurfaceHolder p0, int p1, int p2, int p3) {
    }

    public void surfaceCreated(SurfaceHolder p0) {
        mHasSurface = true;
        mHolder = p0;
    }
}

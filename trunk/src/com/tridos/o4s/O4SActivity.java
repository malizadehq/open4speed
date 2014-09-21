package com.tridos.o4s;

import android.app.Activity;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.RelativeLayout;

import com.google.android.gms.ads.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class O4SActivity extends Activity {

    // paths
    private static final String path = "/data/data/com.tridos.o4s/";
    private static final String configFile = path + "config";
    private static final String versionFile = path + "open4speed.1.0.4";
    // various instances
    private AdView mAdView = null;
    private O4SJNI mO4SJNI;
    public static O4SActivity mO4SActivity;
    // instance of sounds
    private static ArrayList<Sound> list = new ArrayList<Sound>();
    private static MediaPlayer music = new MediaPlayer();
    private static SoundPool snd = new SoundPool(10, AudioManager.STREAM_MUSIC, 0);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // init
        super.onCreate(savedInstanceState);
        mO4SActivity = this;

        // set display layout
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        // keep screen on
        final Window win = getWindow();
        win.addFlags(WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        win.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
                | WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON);

        // set the hardware buttons to control the music
        setVolumeControlStream(AudioManager.STREAM_MUSIC);

        // check if the game is installed
        if (!new File(versionFile).exists()) {
            // install data on sdcard
            try {
                // unpack initial configuration
                if (!new File(configFile).exists()) {
                    InputStream i = getAssets().open("init.zip");
                    new File(path).mkdirs();
                    unzip(i, path);
                    i.close();
                }

                // create version file
                FileOutputStream fout = new FileOutputStream(versionFile);
                fout.write("Created by Luboš Vonásek".getBytes());
                fout.close();
            } catch (IOException ex) {
            }
        }

        // new instance
        if (mO4SJNI == null) {
            mO4SJNI = new O4SJNI(this);
            mAdView = new AdView(this);
            mAdView.setAdUnitId("ca-app-pub-4116662758291059/6202867742");
            mAdView.setAdSize(AdSize.SMART_BANNER);
        }

        // create screen layout
        RelativeLayout layout = new RelativeLayout(mO4SActivity);
        layout.addView(mO4SJNI);

        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams
                (RelativeLayout.LayoutParams.WRAP_CONTENT,
                        RelativeLayout.LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.ALIGN_TOP |
                RelativeLayout.CENTER_HORIZONTAL, RelativeLayout.TRUE);
        if (mAdView != null) {
            layout.addView(mAdView, params);
            AdRequest request = new AdRequest.Builder().addTestDevice(AdRequest.DEVICE_ID_EMULATOR).build();
            mAdView.loadAd(request);
        }

        setContentView(layout);
        mO4SJNI.setRenderer(mO4SJNI);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            switch (event.getKeyCode()) {
                case (21):
                    O4SJNI.nativeKey(100);
                    return true;
                case (22):
                    O4SJNI.nativeKey(102);
                    return true;
                case (19):
                    O4SJNI.nativeKey(101);
                    return true;
                case (20):
                    O4SJNI.nativeKey(103);
                    return true;
                case (62):
                    O4SJNI.nativeKey(160);
                    return true;
                default:
                    O4SJNI.nativeKey(event.getKeyCode());
            }
        }
        if (event.getAction() == KeyEvent.ACTION_UP) {
            switch (event.getKeyCode()) {
                case (21):
                    O4SJNI.nativeKeyUp(100);
                    return true;
                case (22):
                    O4SJNI.nativeKeyUp(102);
                    return true;
                case (19):
                    O4SJNI.nativeKeyUp(101);
                    return true;
                case (20):
                    O4SJNI.nativeKeyUp(103);
                    return true;
                case (62):
                    O4SJNI.nativeKeyUp(160);
                    return true;
                default:
                    O4SJNI.nativeKeyUp(event.getKeyCode());
            }
        }
        return super.dispatchKeyEvent(event);
    }

    @Override
    public void onDestroy() {
        // destroy ad banner
        if (mAdView != null) {
            mAdView.destroy();
        }
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        // pause game
        super.onPause();
        if (mO4SJNI != null) {
            if (mO4SJNI.init) {
                mO4SJNI.paused = true;
                snd.autoPause();
                music.pause();
            }
        }
        // pause ad banner
        if (mAdView != null) {
            mAdView.pause();
        }
    }

    @Override
    protected void onResume() {
        // resume game
        mO4SActivity = this;
        if (mO4SJNI != null) {
            if (mO4SJNI.init) {
                mO4SJNI.paused = false;
                snd.autoResume();
                music.start();
            }
        }
        // resume ad banner
        if (mAdView != null) {
            mAdView.resume();
        }
        super.onResume();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // change back key function
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            O4SJNI.nativeBack();
            return true;
        }
        if (keyCode == KeyEvent.KEYCODE_MENU) {
            O4SJNI.nativeBack();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    public static void soundFreq(int index, float speed) {
        if (list.get(index).id != Integer.MAX_VALUE) {
            list.get(index).rate = speed;
            if (speed < 0.5) {
                list.get(index).rate = 0.5f;
            }
            if (speed > 2.0) {
                list.get(index).rate = 2.0f;
            }
            snd.setRate(list.get(index).id, speed);
        }
    }

    public static void soundLoad(String filename) {
        if (filename.endsWith(".ogg")) {
            try {
                AssetFileDescriptor afd = mO4SActivity.getAssets().openFd(filename);
                Integer id = snd.load(afd, 1);
                list.add(new Sound(id, 1, 0));
            } catch (IOException e) {
            }
        } else {
            if (music != null) {
                music.reset();
            }
            try {
                AssetFileDescriptor afd = mO4SActivity.getAssets().openFd(filename);
                music = new MediaPlayer();
                music.setDataSource(afd.getFileDescriptor(),
                        afd.getStartOffset(), afd.getLength());
                music.prepare();
                music.start();
                list.add(new Sound(Integer.MAX_VALUE, 1, 0));
            } catch (IOException e) {
                music = null;
            }
        }
    }

    public static void soundPlay(int index, int loop) {
        if (list.get(index).id == Integer.MAX_VALUE) {
            if (loop == 1)
                music.setLooping(true);
            else
                music.setLooping(false);
            music.start();
        } else {
            Sound s = list.get(index);
            snd.play(s.id, s.volume, s.volume, 1, 0, s.rate);
        }
    }

    public static void soundStop(int index) {
        if (list.get(index).id == Integer.MAX_VALUE) {
            music.pause();
        } else {
            snd.stop(list.get(index).id);
        }
    }

    public static void soundVolume(int index, float volume) {
        if (list.get(index).id == Integer.MAX_VALUE) {
            music.setVolume(volume, volume);
        } else {
            snd.setVolume(list.get(index).id, volume, volume);
            list.get(index).volume = volume;
        }
    }

    public void unzip(InputStream fin, String location) throws IOException {
        ZipInputStream zin = new ZipInputStream(fin);
        ZipEntry ze = null;

        // parse all files
        while ((ze = zin.getNextEntry()) != null) {
            if (!ze.isDirectory()) {

                // create directory and file
                if (ze.getName().contains("/")) {
                    int lastIndex = ze.getName().lastIndexOf('/');
                    new File(location + ze.getName().substring(0, lastIndex))
                            .mkdirs();
                }
                Log.i("Decompress", location + ze.getName());
                FileOutputStream fout = new FileOutputStream(location
                        + ze.getName());

                // unpack data
                byte[] buffer = new byte[4096];
                int bytes_read;
                while ((bytes_read = zin.read(buffer)) != -1) {
                    fout.write(buffer, 0, bytes_read);
                }

                fout.close();
            }
            zin.closeEntry();
        }
        zin.close();
    }

    static class Sound {

        Integer id;
        float rate;
        float volume;

        Sound(Integer id, float rate, float volume) {
            this.id = id;
            this.rate = rate;
            this.volume = volume;
        }
    }
}
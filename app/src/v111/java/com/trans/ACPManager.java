package com.trans;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Locale;
import java.util.regex.Pattern;

public class ACPManager {
    public static final int MAX_BUFFER = 102400;
    private static final ACPManager mInst = new ACPManager();
    private AssetManager mAssets;
    private String mFilename;
    private String mPrefix;
    private final HashMap<String, String> mFileMap = new HashMap<>();
    private boolean mFileMapReady = false;
    private RawData mBuffer = null;
    private InputStream mStream = null;

    public static class RawData {
        public byte[] data;
        public int length;
    }

    private ACPManager() {
    }

    public static ACPManager getInstance() {
        return mInst;
    }

    public boolean isFile(AssetManager assetManager, String str) {
        boolean z = false;
        try {
            InputStream inputStreamOpen = this.mAssets.open(str);
            z = inputStreamOpen != null;
            if (inputStreamOpen != null) {
                inputStreamOpen.close();
            }
        } catch (Exception unused) {
        }
        return z;
    }

    public boolean listdir(AssetManager assetManager, ArrayList<String> arrayList, String str) {
        try {
            String[] list = assetManager.list(str);
            if (list == null) {
                return false;
            }
            for (String str2 : list) {
                String str3 = str + "/" + str2;
                if (isFile(assetManager, str3)) {
                    arrayList.add(str3);
                } else {
                    if (str.length() != 0) {
                        str2 = str + "/" + str2;
                    }
                    listdir(assetManager, arrayList, str2);
                }
            }
            return true;
        } catch (IOException unused) {
            return false;
        }
    }

    public void setupFileMap(AssetManager assetManager, String str) {
        synchronized (this.mFileMap) {
            this.mFileMap.clear();
            this.mFileMapReady = true;
            if (str.charAt(str.length() - 1) == '/') {
                str = str.substring(0, str.length() - 1);
            }
            ArrayList<String> arrayList = new ArrayList<>();
            listdir(assetManager, arrayList, str);
            for (int i = 0; i < arrayList.size(); i++) {
                String str2 = arrayList.get(i);
                this.mFileMap.put(str2.toLowerCase(Locale.US), str2);
            }
        }
    }

    public void loadFileMap() {
        String str;
        synchronized (this) {
            AssetManager assetManager = this.mAssets;
            if (assetManager != null && (str = this.mPrefix) != null && !this.mFileMapReady) {
                setupFileMap(assetManager, str);
            }
        }
    }

    public void setAssets(AssetManager assetManager, String str) {
        this.mAssets = assetManager;
        this.mPrefix = str;
        this.mFileMapReady = false;
        this.mFileMap.clear();
    }

    public String canonicalPath(String str) {
        String[] strArrSplit = str.split(Pattern.quote(File.separator));
        if (strArrSplit.length < 3) {
            return str;
        }
        String str2 = strArrSplit[strArrSplit.length - 1];
        int length = strArrSplit.length - 2;
        while (length >= 0) {
            if (strArrSplit[length].equals("..") && length > 1) {
                length--;
            } else if (strArrSplit[length].length() > 0 && !strArrSplit[length].equals(".")) {
                str2 = strArrSplit[length] + File.separator + str2;
            }
            length--;
        }
        return str2;
    }

    public String getFileName(String str) {
        String str2;
        String strCanonicalPath = canonicalPath(str);
        String lowerCase = (this.mPrefix + strCanonicalPath).toLowerCase(Locale.US);
        synchronized (this.mFileMap) {
            if (this.mFileMap.containsKey(lowerCase)) {
                str2 = this.mFileMap.get(lowerCase);
            } else {
                str2 = this.mPrefix + strCanonicalPath;
            }
        }
        return str2;
    }

    public boolean hasFile(String str) {
        loadFileMap();
        boolean z = false;
        try {
            InputStream inputStreamOpen = this.mAssets.open(getFileName(str));
            z = inputStreamOpen != null;
            if (inputStreamOpen != null) {
                inputStreamOpen.close();
            }
        } catch (Exception unused) {
        }
        return z;
    }

    public int getFileSize(String str) {
        int iAvailable = 0;
        InputStream inputStreamOpen = null;
        try {
            try {
                inputStreamOpen = this.mAssets.open(getFileName(str));
                iAvailable = inputStreamOpen.available();
                inputStreamOpen.close();
            } catch (IOException unused) {
                if (inputStreamOpen != null) {
                    inputStreamOpen.close();
                }
                return iAvailable;
            } catch (Throwable th) {
                if (inputStreamOpen != null) {
                    try {
                        inputStreamOpen.close();
                    } catch (Exception unused2) {
                    }
                }
                throw th;
            }
        } catch (Exception unused3) {
        }
        return iAvailable;
    }

    public AssetFileDescriptor openFd(String str) {
        try {
            return this.mAssets.openFd(getFileName(str));
        } catch (IOException unused) {
            return null;
        }
    }

    public void beginStream(String str) throws Exception {
        if (this.mStream != null) {
            throw new Exception("beginStream called while another stream is still open");
        }
        String fileName = getFileName(str);
        this.mFilename = fileName;
        try {
            InputStream inputStreamOpen = this.mAssets.open(fileName);
            this.mStream = inputStreamOpen;
            if (inputStreamOpen == null || this.mBuffer != null) {
                return;
            }
            RawData rawData = new RawData();
            this.mBuffer = rawData;
            rawData.data = new byte[MAX_BUFFER];
        } catch (IOException unused) {
        }
    }

    public void endStream() {
        InputStream inputStream = this.mStream;
        if (inputStream != null) {
            try {
                inputStream.close();
            } catch (IOException unused) {
            }
            this.mStream = null;
        }
    }

    public long skip(int i) {
        InputStream inputStream = this.mStream;
        if (inputStream == null) {
            return 0L;
        }
        try {
            return inputStream.skip(i);
        } catch (IOException unused) {
            return 0L;
        }
    }

    public RawData readStream() {
        InputStream inputStream = this.mStream;
        if (inputStream == null) {
            return null;
        }
        try {
            int iAvailable = inputStream.available();
            if (iAvailable > 102400) {
                iAvailable = MAX_BUFFER;
            }
            RawData rawData = this.mBuffer;
            rawData.length = this.mStream.read(rawData.data, 0, iAvailable);
        } catch (IOException unused) {
        }
        return this.mBuffer;
    }
}

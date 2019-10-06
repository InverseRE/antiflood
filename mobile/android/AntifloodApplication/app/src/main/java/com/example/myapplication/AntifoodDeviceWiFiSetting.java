package com.example.myapplication;

import android.content.SharedPreferences;
import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.util.Arrays;

public class AntifoodDeviceWiFiSetting implements AntifoodDeviceSetting {

    private String wifiMode;
    private String wifiChannel;
    private String wifiAuth;
    private String wifiSSID;

    private String wifiModePerfKey = null;
    private String wifiChannelPerfKey = null;
    private String wifiAuthPerfKey = null;
    private String wifiSSIDPerfKey = null;

    private SharedPreferences sharedPreferences;

    private final int ssidMaxLength = 48;
    private final int cmdTotalLength = 55;

    AntifoodDeviceWiFiSetting(String wifiMode, String wifiChannel,String wifiAuth,
                              String wifiSSID) {
        this.wifiMode = wifiMode;
        this.wifiChannel = wifiChannel;
        this.wifiAuth = wifiAuth;
        this.wifiSSID = wifiSSID;
    }

    AntifoodDeviceWiFiSetting(SharedPreferences sp, String wifiModeKey, String wifiChannelKey,
                              String wifiAuthKey, String wifiSSIDKey) {
        sharedPreferences = sp;
        wifiModePerfKey = wifiModeKey;
        wifiChannelPerfKey = wifiChannelKey;
        wifiAuthPerfKey = wifiAuthKey;
        wifiSSIDPerfKey = wifiSSIDKey;
    }

    @Override
    public byte[] load_cmd() { return AntifoodDevice.getWiFiSettingsCommand(); }

    @Override
    public byte[] confirm_cmd() { return AntifoodDevice.storeCurrentSettingsCommand(); }

    @Override
    public byte[] store_cmd() {

        byte[] setWiFiSettings = new byte[cmdTotalLength];
        final byte[] setWiFiSettingsHeader = {0x02, 0x64, 0x00, 0x33};

        System.arraycopy(setWiFiSettingsHeader, 0, setWiFiSettings, 0,
                setWiFiSettingsHeader.length);

        try {
            int mode = Integer.parseInt(wifiMode);
            int channel = Integer.parseInt(wifiChannel);
            int auth = Integer.parseInt(wifiAuth);
            setWiFiSettings[4] = (byte)mode;
            setWiFiSettings[5] = (byte)channel;
            setWiFiSettings[6] = (byte)auth;
        } catch (NumberFormatException e) {
            Log.w("wifi_settings", e.getLocalizedMessage());
        }

        byte[] ssidBytes = wifiSSID.getBytes();
        int ssidLength = ssidBytes.length < ssidMaxLength ? ssidBytes.length : ssidMaxLength;
        System.arraycopy(ssidBytes, 0, setWiFiSettings, 7, ssidLength);

        return setWiFiSettings;
    }

    @Override
    public void parse(byte[] setting, int length) {
        if (length != cmdTotalLength) {
            throw new IllegalArgumentException("bad total length");
        }
        if (setting[0] != 0x03 || setting[1] != 0x64) {
            throw new IllegalArgumentException("bad header data");
        }
        if (setting[2] != 0x00 || setting[3] != 0x33) {
            throw new IllegalArgumentException("bad settings length");
        }

        wifiMode = Integer.toString(setting[4]);
        Log.d("wifi_mode", wifiMode);
        wifiChannel = Integer.toString(setting[5]);
        Log.d("wifi_ch", wifiChannel);
        wifiAuth = Integer.toString(setting[6]);
        Log.d("wifi_auth", wifiAuth);

        String ssid = new String(Arrays.copyOfRange(setting, 7, cmdTotalLength));
        Log.d("wifi_ssid_full", ssid);
        wifiSSID = getString(Arrays.copyOfRange(setting, 7, cmdTotalLength), (byte)0);
        Log.d("wifi_ssid", wifiSSID);
    }

    @Override
    public void fillPreferences() {
        if (wifiModePerfKey != null && wifiChannelPerfKey != null &&
                wifiAuthPerfKey != null && wifiSSIDPerfKey != null) {
            SavePreferences(wifiModePerfKey, wifiMode);
            SavePreferences(wifiChannelPerfKey, wifiChannel);
            SavePreferences(wifiAuthPerfKey, wifiAuth);
            SavePreferences(wifiSSIDPerfKey, wifiSSID);
        }
    }

    @Override
    public void fillPreferences(String msg) {
        if (wifiModePerfKey != null && wifiChannelPerfKey != null &&
                wifiAuthPerfKey != null && wifiSSIDPerfKey != null) {
            SavePreferences(wifiModePerfKey, "Load failed! " + msg);
            SavePreferences(wifiChannelPerfKey, "Load failed! " + msg);
            SavePreferences(wifiAuthPerfKey, "Load failed! " + msg);
            SavePreferences(wifiSSIDPerfKey, "Load failed! " + msg);
        }
    }

    String getString(byte[] arr, byte end) {
        int i;
        for(i = 0; (i < arr.length) && (arr[i] != end); i++);
        return new String(Arrays.copyOfRange(arr,0, i + 1));
    }

    private void SavePreferences(String key, String value) {
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString(key, value);
        editor.commit();
    }
}

package com.example.myapplication;

import android.content.SharedPreferences;
import android.util.Log;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Arrays;

public class AntifoodDeviceServerSetting implements AntifoodDeviceSetting {

    private String srvAddress;
    private String srvPort;

    private String srvIpPrefKey = null;
    private String srvPortPrefKey = null;

    private SharedPreferences sharedPreferences;

    AntifoodDeviceServerSetting(String srvAddress, String srvPort) {
        this.srvAddress = srvAddress;
        this.srvPort = srvPort;
    }

    AntifoodDeviceServerSetting(SharedPreferences sp, String ip, String port) {
        srvIpPrefKey = ip;
        srvPortPrefKey = port;
        sharedPreferences = sp;
    }

    @Override
    public byte[] load_cmd() {
        return AntifoodDevice.getServerSettingsCommand();
    }

    @Override
    public byte[] confirm_cmd() { return AntifoodDevice.storeCurrentSettingsCommand(); }

    @Override
    public byte[] store_cmd() {

        byte[] setServerSettings = {0x02, (byte)0xc9, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        try {
            InetAddress ip = InetAddress.getByName(srvAddress);
            byte[] ipBytes = ip.getAddress();
            System.arraycopy(ipBytes, 0, setServerSettings, 4, ipBytes.length);
        } catch (UnknownHostException e) {
            Log.w("srv_addr", e.getLocalizedMessage());
        }

        try {
            int port = Integer.parseInt(srvPort);
            setServerSettings[9] = (byte)(port >>> 8);
            setServerSettings[8] = (byte)port;
        } catch (NumberFormatException e) {
            Log.w("srv_port", e.getLocalizedMessage());
        }

        return setServerSettings;
    }

    @Override
    public void parse(byte[] setting, int length) {
        if (length != 10) {
            throw new IllegalArgumentException("bad total length");
        }
        if (setting[2] != 0 || setting[3] != 6) {
            throw new IllegalArgumentException("bad settings length");
        }

        srvAddress = myJoin(Arrays.copyOfRange(setting, 4, 8), ".");
        Log.d("srv_addr", srvAddress);

        int port = ((int)(setting[9] & 0xff)  << 8) | ((int)(setting[8] & 0xff));
        srvPort = Integer.toString(port);
        Log.d("srv_port", srvPort);
    }

    @Override
    public void fillPreferences() {
        if (srvIpPrefKey != null && srvPortPrefKey != null) {
            SavePreferences(srvIpPrefKey, srvAddress);
            SavePreferences(srvPortPrefKey, srvPort);
        }
    }

    @Override
    public void fillPreferences(String msg) {
        if (srvIpPrefKey != null && srvPortPrefKey != null) {
            SavePreferences(srvIpPrefKey, "Load failed! " + msg);
            SavePreferences(srvPortPrefKey, "Load failed !" + msg);
        }
    }

    private void SavePreferences(String key, String value) {
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString(key, value);
        editor.commit();
    }

    private static String myJoin(byte[] arr, String separator) {
        if (null == arr || 0 == arr.length) {
            return "";
        }

        StringBuilder sb = new StringBuilder(256);
        sb.append((int)(arr[0] & 0xff));

        for (int i = 1; i < arr.length; i++) {
            sb.append(separator).append((int)(arr[i] & 0xff));
        }

        return sb.toString();
    }
}

package com.example.myapplication;

public class AntifoodDevice {

    // Open valves
    private static final byte[] open_cmd = {0x02, 0x03, 0x00, 0x00};

    // Close valves
    private static final byte[] close_cmd = {0x02, 0x04, 0x00, 0x00};

    // Echo command
    private static final byte[] echo_cmd = {0x00, 0x00, 0x00, 0x00};

    // Get probes state
    private static final byte[] state_cmd = {0x02, 0x02, 0x00, 0x00};

    // Get device time
    private static final byte[] get_time_cmd = {0x02, 0x01, 0x00, 0x03, 0x00, 0x00, 0x00};

    // Get last sync time
    private static final byte[] get_sync_time_cmd = {0x02, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00};

    // Sync time command
    private static final byte[] sync_time_cmd = {0x02, 0x01, 0x00, 0x03, 0x01, 0x00, 0x00};

    // ---------------------

    // Get WIFI settings
    private static final byte[] get_wifi_settings_cmd = {0x02, 0x64, 0x00, 0x00};

    // Get UDP server settings
    private static final byte[] get_srv_settings_cmd = {0x02, 0x65, 0x00, 0x00};

    // Get NTP server settings
    private static final byte[] get_ntp_settings_cmd = {0x02, 0x66, 0x00, 0x00};

    // ---------------------

    // Reload settings from EEPROM
    private static final byte[] set_settings_eeprom_cmd = {0x02, 0x0a, 0x00, 0x01, 0x00};

    // Reload default settings
    private static final byte[] set_settings_default_cmd = {0x02, 0x0a, 0x00, 0x01, 0x01};

    // Store current settings to EEPROM
    private static final byte[] set_current_setting_cmd = {0x02, 0x0b, 0x00, 0x00};

    // ---------------------

    // Reset
    private static final byte[] reset_cmd = {0x02, (byte)0xfc, 0x00, 0x01, 0x00};


    public static String bytesToHexString(byte[] array, int length) {
        String hexString = "";
        for (int i = 0; i < length; i++) {
            String thisByte = "".format("%02x", array[i]);
            hexString += thisByte;
        }
        return hexString;
    }

    public static String bytesToHexString(byte[] array) {
        return bytesToHexString(array, array.length);
    }

    public static byte[] getOpenCommand() {
        return open_cmd;
    }

    public static byte[] getCloseCommand() {
        return close_cmd;
    }

    public static byte[] getEchoCommand() {
        return echo_cmd;
    }

    public static byte[] getStateCommand() {
        return state_cmd;
    }

    public static byte[] getTimeCommand() {
        return get_time_cmd;
    }

    public static byte[] getLastSyncCommand() {
        return get_sync_time_cmd;
    }

    public static byte[] syncTimeCommand() {
        return sync_time_cmd;
    }



    public static byte[] getWiFiSettingsCommand() { return get_wifi_settings_cmd; }

    public static byte[] getServerSettingsCommand() {
        return get_srv_settings_cmd;
    }

    public static byte[] getNtpSettingsCommand() { return get_ntp_settings_cmd; }



    public static byte[] loadEEPROMSettingsCommand() { return set_settings_eeprom_cmd; }

    public static byte[] loadDefaultSettingsCommand() { return set_settings_default_cmd; }

    public static byte[] storeCurrentSettingsCommand() { return set_current_setting_cmd; }



    public static byte[] resetCommand() {
        return reset_cmd;
    }

}

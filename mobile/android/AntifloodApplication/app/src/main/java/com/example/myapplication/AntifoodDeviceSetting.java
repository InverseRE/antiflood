package com.example.myapplication;

interface AntifoodDeviceSetting {

    /* Load bytes from device. */
    byte[] load_cmd();

    /* Store bytes from device. */
    byte[] store_cmd();

    /* Write to EEPROM. */
    public byte[] confirm_cmd();

    /* Parse settings in human readable representation. */
    void parse(byte[] setting, int length);

    /* Update Preferences -> Ok. */
    void fillPreferences();

    /* Update Preferences -> With Error message. */
    void fillPreferences(String msg);
}

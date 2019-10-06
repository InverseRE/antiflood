package com.example.myapplication;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.MenuItem;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;

public class DeviceSettingsActivity extends AppCompatActivity {

    private DeviceSettingsFragment settingsFragment;

    private MessageSender cmdSender;
    private SharedPreferences.OnSharedPreferenceChangeListener listener;

    SharedPreferences sharedPref;

    private static final String DefaultHostAddress = "192.168.0.111";
    private static final String DefaultHostPort = "80";
    private static final String DefaultSocketTimeout = "5000";

    private static final int MaxPacketLength = 1500;

    private static String hostAddress;
    private static int hostPort;
    private static int socketTimeout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.device_settings_activity);
        settingsFragment = new DeviceSettingsFragment();
        getSupportFragmentManager()
                .beginTransaction()
                .replace(R.id.device_settings, settingsFragment)
                .commit();
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        // Settings change listener
        listener = new SharedPreferences.OnSharedPreferenceChangeListener() {
            public void onSharedPreferenceChanged(SharedPreferences prefs, String key) {
                switch(key) {
                    case "host_ip":
                        hostAddress = prefs.getString(key, DefaultHostAddress);
                        break;
                    case "host_port":
                        hostPort = Integer.valueOf(prefs.getString(key, DefaultHostPort));
                        break;
                    case "socket_timeout":
                        socketTimeout = Integer.valueOf(prefs.getString(key, DefaultSocketTimeout));
                        cmdSender = new MessageSender(MaxPacketLength, socketTimeout);
                        break;
                }
            }
        };

        sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
        sharedPref.registerOnSharedPreferenceChangeListener(listener);

        hostAddress = sharedPref.getString("host_ip", DefaultHostAddress);
        hostPort = Integer.valueOf(sharedPref.getString("host_port", DefaultHostPort));
        socketTimeout = Integer.valueOf(sharedPref.getString("socket_timeout", DefaultSocketTimeout));

        cmdSender = new MessageSender(MaxPacketLength, socketTimeout);
    }

    @Override
    protected void onResume() {
        super.onResume();

        // Load function
        Preference load_pref = settingsFragment.findPreference(getString(R.string.load_eeprom_key));
        load_pref.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
            @Override
            public boolean onPreferenceClick(Preference preference) {
                AntifoodDeviceServerSetting srvSetting = new AntifoodDeviceServerSetting(
                        sharedPref,
                        getString(R.string.server_ip_key),
                        getString(R.string.server_port_key));

                AntifoodDeviceWiFiSetting wifiSetting = new AntifoodDeviceWiFiSetting(
                        sharedPref,
                        getString(R.string.wifi_mode_key),
                        getString(R.string.wifi_channel_key),
                        getString(R.string.wifi_auth_key),
                        getString(R.string.wifi_ssid_key));

                //TODO: hardcoded delay
                cmdSender.load(0, hostAddress, hostPort, srvSetting, preference);
                cmdSender.load(1000, hostAddress, hostPort, wifiSetting, preference);
                return true;
            }
        });

        // Store function
        Preference store_pref = settingsFragment.findPreference(getString(R.string.store_eeprom_key));
        store_pref.setOnPreferenceClickListener(new Preference.OnPreferenceClickListener() {
            @Override
            public boolean onPreferenceClick(Preference preference) {
                String srvAddress = sharedPref.getString(getString(R.string.server_ip_key), "");
                String srvPort = sharedPref.getString(getString(R.string.server_port_key), "");

                AntifoodDeviceServerSetting srvSetting = new AntifoodDeviceServerSetting(
                        srvAddress, srvPort);

                cmdSender.store(hostAddress, hostPort, srvSetting, preference);
                return true;
            }
        });
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch (id) {
            // Respond to the action bar's Up/Home button
            case android.R.id.home:
                super.onBackPressed();
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    public static class DeviceSettingsFragment extends PreferenceFragmentCompat {
        @Override
        public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
            setPreferencesFromResource(R.xml.device_preferences, rootKey);
        }
    }
}
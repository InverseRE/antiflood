package com.example.myapplication;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.content.ContextCompat;

import android.preference.PreferenceManager;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;

import android.widget.Button;
import android.widget.TextView;

import android.Manifest;
import android.content.pm.PackageManager;

public class MainActivity extends AppCompatActivity {

    private static final String DefaultHostAddress = "192.168.0.111";
    private static final String DefaultHostPort = "80";
    private static final String DefaultSocketTimeout = "5000";

    private static final int MaxPacketLength = 1500;

    private static String hostAddress;
    private static int hostPort;
    private static int socketTimeout;

    private MessageSender cmdSender;
    private SharedPreferences.OnSharedPreferenceChangeListener listener;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        TextView logsView = findViewById(R.id.logs);
        logsView.setMovementMethod(new ScrollingMovementMethod());

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.INTERNET)
                == PackageManager.PERMISSION_GRANTED) {
            logsView.append("\n INTERNET perm granted!");
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CHANGE_NETWORK_STATE)
                == PackageManager.PERMISSION_GRANTED) {
            logsView.append("\n CHANGE_NETWORK_STATE perm granted!");
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_NETWORK_STATE)
                == PackageManager.PERMISSION_GRANTED) {
            logsView.append("\n ACCESS_NETWORK_STATE perm granted!");
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CHANGE_WIFI_STATE)
                == PackageManager.PERMISSION_GRANTED) {
            logsView.append("\n CHANGE_WIFI_STATE perm granted!");
        }
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_WIFI_STATE)
                == PackageManager.PERMISSION_GRANTED) {
            logsView.append("\n ACCESS_WIFI_STATE perm granted!");
        }

        // Settings change listener
        listener = new SharedPreferences.OnSharedPreferenceChangeListener() {
            public void onSharedPreferenceChanged(SharedPreferences prefs, String key) {
                TextView logsView = findViewById(R.id.logs);
                switch(key) {
                    case "host_ip_addr":
                        hostAddress = prefs.getString("host_ip", DefaultHostAddress);
                        break;
                    case "host_port":
                        hostPort = Integer.valueOf(prefs.getString("host_port",
                                DefaultHostPort));
                        break;
                    case "socket_timeout":
                        socketTimeout = Integer.valueOf(prefs.getString("socket_timeout",
                                DefaultSocketTimeout));
                        cmdSender = new MessageSender(MaxPacketLength, socketTimeout);
                        break;
                }
                logsView.append("\n settings updated!");
            }
        };

        SharedPreferences sharedPref =
                PreferenceManager.getDefaultSharedPreferences(this);
        sharedPref.registerOnSharedPreferenceChangeListener(listener);

        hostAddress = sharedPref.getString("host_ip", DefaultHostAddress);
        hostPort = Integer.valueOf(sharedPref.getString("host_port", DefaultHostPort));
        socketTimeout = Integer.valueOf(sharedPref.getString("socket_timeout", DefaultSocketTimeout));

        cmdSender = new MessageSender(MaxPacketLength, socketTimeout);

        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "HostIP -> " + hostAddress + " : " + hostPort + " \n" +
                        "Socket Timeout -> " + socketTimeout + " ms", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        FloatingActionButton fab_ver = findViewById(R.id.fab_version);
        fab_ver.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Antiflood Copyright (C) 2019"
                        + "\n Version: " + BuildConfig.VERSION_NAME, Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        // Open Button function
        Button open_button = findViewById(R.id.button_open);
        final byte[] open_cmd = AntifoodDevice.getOpenCommand();
        open_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                TextView logsView = findViewById(R.id.logs);
                logsView.append("\n send open");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(open_cmd));
                cmdSender.send(hostAddress, hostPort, open_cmd, logsView);
            }
        });

        // Close Button function
        Button close_button = findViewById(R.id.button_close);
        final byte[] close_cmd = AntifoodDevice.getCloseCommand();
        close_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                TextView logsView = findViewById(R.id.logs);
                logsView.append("\n send close");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(close_cmd));
                cmdSender.send(hostAddress, hostPort, close_cmd, logsView);
            }
        });

        // Echo Button function
        Button echo_button = findViewById(R.id.button_echo);
        final byte[] echo_cmd = AntifoodDevice.getEchoCommand();
        echo_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                TextView logsView = findViewById(R.id.logs);
                logsView.append("\n send echo");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(echo_cmd));
                cmdSender.send(hostAddress, hostPort, echo_cmd, logsView);
            }
        });

        // State Button function
        Button state_button = findViewById(R.id.button_state);
        final byte[] state_cmd = AntifoodDevice.getStateCommand();
        state_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                TextView logsView = findViewById(R.id.logs);
                logsView.append("\n send get state");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(state_cmd));
                cmdSender.send(hostAddress, hostPort, state_cmd, logsView);
            }
        });

        // Get Time Button function
        Button time_button = findViewById(R.id.button_time);
        final byte[] get_time_cmd = AntifoodDevice.getTimeCommand();
        final byte[] get_sync_time_cmd = AntifoodDevice.getLastSyncCommand();
        time_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                TextView logsView = findViewById(R.id.logs);
                logsView.append("\n send get current time");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(get_time_cmd));
                cmdSender.send(hostAddress, hostPort, get_time_cmd, logsView);
                logsView.append("\n send get last sync time");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(get_sync_time_cmd));
                cmdSender.send(hostAddress, hostPort, get_sync_time_cmd, logsView);
            }
        });

        // Sync Time Button function
        Button sync_button = findViewById(R.id.button_sync);
        final byte[] sync_time_cmd = AntifoodDevice.syncTimeCommand();
        sync_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                TextView logsView = findViewById(R.id.logs);
                logsView.append("\n send time sync");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(sync_time_cmd));
                cmdSender.send(hostAddress, hostPort, sync_time_cmd, logsView);
            }
        });

        // Reset Button
        Button rst_button = findViewById(R.id.button_reset);
        final byte[] rst_cmd = AntifoodDevice.resetCommand();
        rst_button.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                TextView logsView = findViewById(R.id.logs);
                logsView.append("\n send reset");
                logsView.append("\n " + AntifoodDevice.bytesToHexString(rst_cmd));
                cmdSender.send(hostAddress, hostPort, rst_cmd, logsView);
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //Program settings
        if (id == R.id.action_settings) {
            Intent intent = new Intent(MainActivity.this, SettingsActivity.class);
            startActivity(intent);
            return true;
        }

        //Device settings
        if (id == R.id.action_device_settings) {
            Intent intent = new Intent(MainActivity.this, DeviceSettingsActivity.class);
            startActivity(intent);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}

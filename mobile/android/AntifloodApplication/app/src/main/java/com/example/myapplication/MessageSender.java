package com.example.myapplication;

import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import androidx.preference.Preference;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;

import static java.lang.Thread.sleep;

public class MessageSender {

    private static int socketTimeout;
    private static int maxPacketLength;

    MessageSender(int length, int timeout) {
        maxPacketLength = length;
        socketTimeout = timeout;
    }

    public static void send(final String hostIP, final int hostPort, final byte[] message,
                             final TextView logger) {

        final Handler handler = new Handler();
        Thread thread = new Thread(new Runnable() {

            String response = null;

            @Override
            public void run() {
                DatagramSocket ds = null;
                try {
                    ds = new DatagramSocket();
                    ds.setSoTimeout(socketTimeout);
                    InetAddress serverAddr = InetAddress.getByName(hostIP);
                    DatagramPacket dp = new DatagramPacket(message, message.length,
                            serverAddr, hostPort);
                    ds.send(dp);

                    byte[] buff = new byte[maxPacketLength];
                    dp = new DatagramPacket(buff, buff.length);
                    ds.receive(dp);
                    response = AntifoodDevice.bytesToHexString(buff, dp.getLength());
                } catch (SocketTimeoutException e) {
                    e.printStackTrace();
                    response = e.getLocalizedMessage();
                } catch (Exception e) {
                    e.printStackTrace();
                    response = e.getLocalizedMessage();
                } finally {
                    if (ds != null) {
                        ds.close();
                    }
                }

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (response != null) {
                            if (logger != null) {
                                logger.append("\n Response : " + response);
                            }
                        } else {
                            if (logger != null) {
                                logger.append("\n null ");
                            }
                        }
                    }
                });
            }
        });

        thread.start();
    }

    public static void load(final int delay, final String hostIP, final int hostPort,
                            final AntifoodDeviceSetting setting, final Preference loadButton) {

        final Handler handler = new Handler();
        Thread thread = new Thread(new Runnable() {

            String response = null;

            @Override
            public void run() {
                DatagramSocket ds = null;
                try {
                    sleep(delay);
                    ds = new DatagramSocket();
                    ds.setSoTimeout(socketTimeout);
                    InetAddress serverAddr = InetAddress.getByName(hostIP);
                    final byte[] message = setting.load_cmd();
                    Log.d("load_msg", AntifoodDevice.bytesToHexString(message, message.length));
                    DatagramPacket dp = new DatagramPacket(message, message.length,
                            serverAddr, hostPort);
                    ds.send(dp);

                    byte[] buff = new byte[maxPacketLength];
                    dp = new DatagramPacket(buff, buff.length);
                    ds.receive(dp);
                    Log.d("load_resp", AntifoodDevice.bytesToHexString(buff, dp.getLength()));
                    setting.parse(buff, dp.getLength());
                } catch (SocketTimeoutException e) {
                    e.printStackTrace();
                    response = e.getLocalizedMessage();
                } catch (Exception e) {
                    e.printStackTrace();
                    response = e.getLocalizedMessage();
                } finally {
                    if (ds != null) {
                        ds.close();
                    }
                }

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (response != null) {
                            // ERROR
                            setting.fillPreferences(response);
                            loadButton.setSummary("Loading settings failed!");
                        } else {
                            //OK
                            setting.fillPreferences();
                            loadButton.setSummary("Settings loaded successfully!");
                        }
                    }
                });
            }
        });

        thread.start();
    }

    public static void store(final String hostIP, final int hostPort,
                            final AntifoodDeviceSetting setting, final Preference storeButton) {

        final Handler handler = new Handler();
        Thread thread = new Thread(new Runnable() {

            String response = null;

            @Override
            public void run() {
                DatagramSocket ds = null;
                try {
                    ds = new DatagramSocket();
                    ds.setSoTimeout(socketTimeout);
                    InetAddress serverAddr = InetAddress.getByName(hostIP);
                    final byte[] message = setting.store_cmd();
                    Log.d("store_msg", AntifoodDevice.bytesToHexString(message, message.length));
                    DatagramPacket dp = new DatagramPacket(message, message.length,
                            serverAddr, hostPort);
                    ds.send(dp);

                    byte[] buff = new byte[maxPacketLength];
                    dp = new DatagramPacket(buff, buff.length);
                    ds.receive(dp);
                    Log.d("store_resp", AntifoodDevice.bytesToHexString(buff, dp.getLength()));

                    final byte[] confirm = setting.confirm_cmd();
                    Log.d("conf_msg", AntifoodDevice.bytesToHexString(confirm, confirm.length));
                    dp = new DatagramPacket(confirm, confirm.length, serverAddr, hostPort);
                    ds.send(dp);
                    dp = new DatagramPacket(buff, buff.length);
                    ds.receive(dp);
                    Log.d("conf_resp", AntifoodDevice.bytesToHexString(buff, dp.getLength()));

                } catch (SocketTimeoutException e) {
                    e.printStackTrace();
                    response = e.getLocalizedMessage();
                } catch (Exception e) {
                    e.printStackTrace();
                    response = e.getLocalizedMessage();
                } finally {
                    if (ds != null) {
                        ds.close();
                    }
                }

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        if (response != null) {
                            // ERROR
                            storeButton.setSummary("Storing settings failed!");
                        } else {
                            //OK
                            storeButton.setSummary("Settings stored successfully!");
                        }
                    }
                });
            }
        });

        thread.start();
    }
}

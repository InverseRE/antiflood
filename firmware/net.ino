/* -*- mode: c -*- */

/*
   Antiflood Copyright (C) 2018 Alexey <Inverse> Shumeiko

   This file is part of Antiflood project.

   This firmware is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This firmware is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the firmware. If not, see <http://www.gnu.org/licenses/>.

   For more details see LICENSE file.
*/

#include <WiFiEspClient.h>
#include <WiFiEspServer.h>

#include "net.h"
#include "web.h"
#include "valve.h"
#include "app.h"



#define SHIELD_STARTUP_TIME     1000        /**< startup time for ESP8266, ms */
#define SHIELD_BAUD_RATE        115200      /**< shield's UART baud rate */
#define SHIELD_PWR_SAVE_ACTION  2           /**< turn off WIFI & activate power save mode action identifier */

#define WIFI_SSID               WIFI_DEFAULT_SSID
#define WIFI_PASS               WIFI_DEFAULT_PASS
#define WIFI_CHAN               WIFI_DEFAULT_CHAN
#define WIFI_SECU               WIFI_DEFAULT_SECU

#define WEB_IP                  APP_DEFAULT_IP
#define WEB_IP_STR              APP_DEFAULT_IP_S
#define WEB_PORT                APP_DEFAULT_PORT
#define WEB_TRX_LATENCY         10          /**< some delays in web communication, ms */
#define WEB_IN_CACHE_SIZE       32          /**< size of input buffer for http */



/** WEB server. */
static WiFiEspServer Server(WEB_PORT);

/** Buffered reading for Web-requests. */
static RingBuffer ibuff(WEB_IN_CACHE_SIZE);

/** WIFI shield state. true - running, false - stoped */
bool wifi_shield_state = false; /* TODO: should be static,
                                         but there is extern reference from power.ino */

/** Configure web-server. */
void web_configure(void)
{
    /* Start shield. */
    pinMode(WIFIEN, OUTPUT);
    digitalWrite(WIFIEN, HIGH);
    pinMode(WIFIRS, OUTPUT);
    digitalWrite(WIFIRS, HIGH);

    delay(SHIELD_STARTUP_TIME);

    Serial.begin(SHIELD_BAUD_RATE);
    WiFi.init(&Serial);
    if (WiFi.status() == WL_NO_SHIELD) {
        DP("esp8266 shield is not responsive");
        power_save_mode_on();
        return;
    }
    DP("esp8266 online");

    /* Configure network. */
    IPAddress ip_addr(WEB_IP);

#if defined WIFI_STATION
    WiFi.config(ip_addr);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
#elif defined WIFI_ACCESS_POINT
    WiFi.configAP(ip_addr);
    WiFi.beginAP(WIFI_SSID, WIFI_CH, WIFI_PASS, WIFI_SECU);
#else
#   error WiFi unknown mode
#endif
    DP("wifi configured");

    /* Start server. */
    Server.begin();
    /* Set WIFI flag. */
    wifi_shield_state = true;
    DP("web-server started");
}

/** Process web-server events. */
void web_run(void)
{
    WiFiEspClient client = Server.available();

    if (client) {
        String rcv_url = "";
        bool stage_1 = true;
        bool stage_2 = false;

        DP("client appears");

        ibuff.init();

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                ibuff.push(c);

                /* Search for GET request. */
                if (stage_1 && ibuff.endsWith("GET ")) {
                    stage_1 = false;
                    stage_2 = true;
                    continue;
                }

                /* Store requested URL. */
                if (stage_2) {
                    rcv_url += c;
                    if (c == ' ') {
                        stage_2 = false;
                        rcv_url.trim();
                    }
                }

                /* Two newline characters in a row is the end of the HTTP request. */
                if (ibuff.endsWith("\r\n\r\n")) {
                    DP("data received");
                    if (!stage_1 && !stage_2) {
                        http_parse_request(client, rcv_url);
                    } else {
                        /* Let me know if GET request not parsed. Back to the main page in 1 second. */
                        http_action_response(client, -2, 1);
                    }
                    DP("data sent");
                    break;
                }
            }
        }
        if (!client.connected()) {
            DP("client disconnected");
            delay(1);
        }

        /* Time to complete transmission. */
        delay(WEB_TRX_LATENCY);

        /* Disconnect. */
        client.stop();
        DP("client dropped");
    }
}

/** Stop communications. */
void web_deactivate(void)
{
    WiFi.disconnect();
    delay(10);
    pinMode(WIFIEN, OUTPUT);
    digitalWrite(WIFIEN, LOW);
    pinMode(WIFIRS, OUTPUT);
    digitalWrite(WIFIRS, LOW);
}

/** Return actual wifi shield state. */
bool get_wifi_state(void)
{
    return wifi_shield_state;
}

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

#include "config.h"
#include "net.h"

#define SHIELD_BAUD_RATE     115200         /**< shield's UART baud rate */
#define WEB_IN_CACHE_SIZE    32             /**< size of input buffer */

NetServer::NetServer(const Ticker& ticker,
        IPAddress ip, short port,
        const String& ssid, const String& password)
        : _ticker(ticker),
          _mode(WIFI_STATION),
          _server(port),
          _ibuff(WEB_IN_CACHE_SIZE),
          _request("")
{
    /* Start shield HW. */
    pinMode(WIFIEN, OUTPUT);
    pinMode(WIFIRS, OUTPUT);
    digitalWrite(WIFIEN, HIGH);
    digitalWrite(WIFIRS, HIGH);

    _ticker.delay_shield_up();

    /* Establish connection. */
    Serial.begin(SHIELD_BAUD_RATE);
    WiFi.init(&Serial);
    if (WiFi.status() == WL_NO_SHIELD) {
        _is_online = false;
        return;
    }

    /* Register in a network. */
    WiFi.config(ip_addr);
    WiFi.begin(ssid, password);

    /* Start server. */
    _server.begin();
    _is_online = true;
}

NetServer::NetServer(const Ticker& ticker,
        IPAddress ip, short port,
        const String& ssid, const String& password,
        int channel, int auth_type);
        : _ticker(ticker),
          _mode(WIFI_ACCESS_POINT),
          _server(port),
          _ibuff(WEB_IN_CACHE_SIZE),
          _request("")
{
    /* Start shield HW. */
    pinMode(WIFIEN, OUTPUT);
    pinMode(WIFIRS, OUTPUT);
    digitalWrite(WIFIEN, HIGH);
    digitalWrite(WIFIRS, HIGH);

    _ticker.delay_shield_up();

    /* Establish connection. */
    Serial.begin(SHIELD_BAUD_RATE);
    WiFi.init(&Serial);
    if (WiFi.status() == WL_NO_SHIELD) {
        _is_online = false;
        return;
    }

    /* Register in a network. */
    WiFi.configAP(ip_addr);
    WiFi.beginAP(ssid, channel, password, auth_type);

    /* Start server. */
    _server.begin();
    _is_online = true;
}

void NetServer::disconnect(void)
{
    /* Turn off WIFI library. */
    WiFi.disconnect();

    _ticker.delay_shield_down();

    /* Turn off WIFI shield. */
    digitalWrite(WIFIEN, LOW);
    digitalWrite(WIFIRS, LOW);

    _is_online = false;
}

const String& NetServer::run(WiFiEspClient& client)
{
    bool stage_1 = true;
    bool stage_2 = false;
    _request = "";

    client = Server.available();
    if (!client) {
        return _request;
    }

    _ibuff.init();

    while (client.connected()) {

        /* Wait till data appears. */
        if (!client.available()) {
            continue;
        }

        char c = client.read();
        _ibuff.push(c);

        /* Search for GET request. */
        if (stage_1 && _ibuff.endsWith("GET ")) {
            stage_1 = false;
            stage_2 = true;
            continue;
        }

        /* Store requested URL. */
        if (stage_2) {
            _request += c;
            if (c == ' ') {
                stage_2 = false;
                _request.trim();
            }
        }

        /* Two newline characters in a row are the end of the HTTP request. */
        if (ibuff.endsWith("\r\n\r\n")) {
            return !stage_1 && !stage_2 ? _request : _request = "";
        }
    }

    return _request = "";
}

void NetServer::suspend(void)
{
    /* Turn off WIFI library. */
    WiFi.disconnect();

    _ticker.delay_shield_down();

    /* Turn off WIFI shield. */
    /* TODO: keep ESP8266 operational */
    digitalWrite(WIFIEN, LOW);
    digitalWrite(WIFIRS, LOW);

    _is_online = false;
}

void NetServer::resume(void)
{
    /* TODO: restore ESP8266 operations */
    _is_online = true;
}

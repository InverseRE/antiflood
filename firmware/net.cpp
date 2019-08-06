/* -*-*- mode: c++ -*-*- */

/*
   Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko

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
#include "debug.h"
#include "hw.h"
#include "net.h"
#include "debug.h"

#define SHIELD_BAUD_RATE     115200         /**< shield's UART baud rate */

NetServer::NetServer(const Ticker& ticker,
        IPAddress ip, short port,
        const String& ssid, const String& password)
        : _ticker(ticker),
          _mode(STYPE_STATION),
          _ip(ip),
          _port(port),
          _ssid(ssid),
          _password(password),
          _channel(0),
          _auth_type(0),
          _is_online(false),
          _is_sending(false)
{
}

NetServer::NetServer(const Ticker& ticker,
        IPAddress ip, short port,
        const String& ssid, const String& password,
        int channel, int auth_type)
        : _ticker(ticker),
          _mode(STYPE_ACCESS_POINT),
          _ip(ip),
          _port(port),
          _ssid(ssid),
          _password(password),
          _channel(channel),
          _auth_type(auth_type),
          _is_online(false),
          _is_sending(false)
{
}

void NetServer::setup(void)
{
    /* Start shield HW. */
    dPC("net: power up ESP8266");

    pinMode(WIFIEN, OUTPUT);
    pinMode(WIFIRS, OUTPUT);
    digitalWrite(WIFIEN, HIGH);
    digitalWrite(WIFIRS, HIGH);

    /* Establish connection. */
    dPC("net: init WiFi");

    Serial.begin(SHIELD_BAUD_RATE);
    WiFi.init(&Serial);
    wdt_reset();

    if (WiFi.status() == WL_NO_SHIELD) {
        iPC("net: no response, offline");
        _is_online = false;
        return;
    }

    /* Register in a network. */
    if (STYPE_STATION == _mode) {
        iPC("net: S-mode");
        WiFi.config(_ip);
        wdt_reset();
        WiFi.begin(_ssid.c_str(), _password.c_str());
        wdt_reset();
        iPV("net: ssid", _ssid);
        iPV("net: ip", _ip);
        iPV("net: port", _port);
    }
    if (STYPE_ACCESS_POINT == _mode) {
        iPC("net: AP-mode");
        WiFi.configAP(_ip);
        wdt_reset();
        WiFi.beginAP(_ssid.c_str(), _channel, _password.c_str(), _auth_type);
        wdt_reset();
        dPV("net: ch", _channel);
        iPV("net: ssid", _ssid);
        iPV("net: ip", _ip);
        iPV("net: port", _port);
    }
    if (STYPE_DUAL == _mode) {
        iPC("net: D-mode");
        uPC("NET: USUPPORTED");
    }

    /* Start server. */
    _is_online = _udp.begin(_port);
    _is_sending = false;
    wdt_reset();

    dPV("net: setup", _is_online);
}

void NetServer::disconnect(void)
{
    dPC("net: disconnect");

    /* Turn off server. */
    _udp.stop();

    /* Turn off WIFI library. */
    WiFi.disconnect();

    /* Turn off WIFI shield. */
    digitalWrite(WIFIEN, LOW);
    digitalWrite(WIFIRS, LOW);

    _is_online = false;
    _is_sending = false;
}

void NetServer::suspend(void)
{
    dPC("net: suspend");

    /* Turn off server. */
    _udp.stop();

    /* Turn off WIFI library. */
    WiFi.disconnect();

    /* Turn off WIFI shield. */
    /* TODO: keep ESP8266 operational */
    digitalWrite(WIFIEN, LOW);
    digitalWrite(WIFIRS, LOW);

    _is_online = false;
    _is_sending = false;
}

void NetServer::resume(void)
{
    dPC("net: resume");

    /* TODO: restore ESP8266 operations */
    _is_online = true;
    _is_sending = false;
}

bool NetServer::rx(void)
{
    bool conn = _udp.parsePacket();
    if (conn) {
        dPC("net: rx");

        IPAddress ipa = _udp.remoteIP();
        uint16_t rp = _udp.remotePort();
        dPV("net: dx ip", ipa);
        dPV("net: dx port", rp);
    }
    return conn;
}

int NetServer::available(void)
{
    return _udp.available();
}

int NetServer::read(void* buf, int len)
{
    return _udp.read((byte*)buf, len);
}

void NetServer::write(const void* buf, int len)
{
    if (!_is_sending) {
        dPC("net: tx");

        _is_sending = true;
        // FIXME: _udp.remotePort() from Arduino/libraries/WiFiEsp/src/utility/EspDrv.cpp
        // comment out the second serial read (at EspDrv.cpp:686)
        // https://github.com/bportaluri/WiFiEsp/issues/119
        IPAddress ipa = _udp.remoteIP();
        uint16_t rp = _udp.remotePort();
        _udp.beginPacket(ipa, rp);
        dPV("net: dx ip", ipa);
        dPV("net: dx port", rp);
    }
    _udp.write((byte*)buf, len);
}


void NetServer::tx(void)
{
    if (_is_sending) {
        dPC("net: tx stop");

        _is_sending = false;
        _udp.endPacket();
    }
}

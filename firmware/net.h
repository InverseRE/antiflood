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

#ifndef __NET_H__
#define __NET_H__

#include <Arduino.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include "ticker.h"

enum WiFiType {
    STYPE_DUAL,
    STYPE_ACCESS_POINT,
    STYPE_STATION
};

class NetServer {
private:
    const Ticker& _ticker;
    const WiFiType _mode;
    const IPAddress _ip;
    const short _port;
    const String _ssid;
    const String _password;
    const int _channel;
    const int _auth_type;
    WiFiEspUDP _udp;
    bool _is_online;
    bool _is_sending;

public:
    NetServer(const Ticker& ticker,
            IPAddress ip, short port,
            const String& ssid, const String& password);
    NetServer(const Ticker& ticker,
            IPAddress ip, short port,
            const String& ssid, const String& password,
            int channel, int auth_type);
    void setup(void);

    bool is_online(void) const { return _is_online; }
    bool is_offline(void) const { return !_is_online; }
    WiFiType mode(void) const { return _mode; }
    void disconnect(void);
    void suspend(void);
    void resume(void);

    bool rx(void);
    int available(void);
    int read(void* buf, int len);

    void write(const void* buf, int len);
    void tx(void);
};

#endif  /* __NET_H__ */

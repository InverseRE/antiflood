/* -*- mode: c++ -*- */

/*
   Antiflood Copyright (C) 2019 Alexey <SmallShark> Khomyakovsky
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

#include "ntp.h"
#include "debug.h"
#include "hw.h"

#define NTP_PACKET_SIZE 48

bool NTP::sync(NetServer& net, const char* pool, unsigned short port, unsigned short wait_time)
{
    dPA("ntp: pool", pool, strlen(pool));
    dPV("ntp: port", port);
    dPV("ntp: wait", wait_time);

    byte packet[NTP_PACKET_SIZE];

    // send request
    byte len = make_ntp_request(packet, sizeof(packet));

    net.write(pool, port, packet, len);
    net.tx();
    memset(packet, 0, sizeof(packet));

    // wait for a response
    unsigned long mark = _ticker.mark();

    while (!net.rx() && _ticker.mark() - mark < wait_time);

    // parse response
    if (0 == net.available()) {
        dPC("ntp: no response");
        return false;
    }

    len = net.read(packet, sizeof(packet));
    dPA("ntp: response", packet, len);
    unsigned long epoch = parse_ntp_response(packet, sizeof(packet));

    if (0 == epoch) {
        dPC("ntp: malformed response");
        return false;
    } else {
        _epoch = epoch;
    }

    return true;
}

unsigned long NTP::get_utc_epoch(bool current_time) const
{
    unsigned long t = current_time ? _epoch + _ticker.mark() / 1000 : _epoch;
    return t;
}

unsigned short NTP::get_utc_hours(bool current_time) const
{
    unsigned long t = current_time ? _epoch + _ticker.mark() / 1000 : _epoch;
    return (t % 86400) / 3600;
}

byte NTP::get_utc_minutes(bool current_time) const
{
    unsigned long t = current_time ? _epoch + _ticker.mark() / 1000 : _epoch;
    return (t % 3600) / 60;
}

byte NTP::get_utc_seconds(bool current_time) const
{
    unsigned long t = current_time ? _epoch + _ticker.mark() / 1000 : _epoch;
    return (t % 60) / 1;
}

byte NTP::make_ntp_request(byte* buff, byte len)
{
    if (len < NTP_PACKET_SIZE) {
        return 0;
    }

    memset(buff, 0, NTP_PACKET_SIZE);

    buff[0] = 0xe3;                         // LI (leap indicator)
    buff[1] = 0x00;                         // clock type
    buff[2] = 0x06;                         // request interval
    buff[3] = 0xec;                         // precission
    buff[12] = 0x31;
    buff[13] = 0x4e;
    buff[14] = 0x31;
    buff[15] = 0x34;

    return NTP_PACKET_SIZE;
}

unsigned long NTP::parse_ntp_response(const byte* buff, byte len)
{
    if (len < NTP_PACKET_SIZE) {
        return 0;
    }

    unsigned long hw = word(buff[40], buff[41]);
    unsigned long lw = word(buff[42], buff[43]);

    // time since 1.01.1900 in seconds
    unsigned long orig = hw << 16 | lw;

    // convert NTP time to Unix time
    // Unix time starts from 1.01.1970; dt = 2208988800 sec
    return orig - 2208988800UL;
}

/* -*- mode: c -*- */

/*
   Antiflood Copyright (C) 2019 Alexey <SmallShark> Khomyakovsky

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

#include <assert.h>
#include "debug.h"
#include "ntp_time.h"

/** Send NTP reguest. */
byte ntp_make_request(byte* buff, byte len)
{
  assert(len >= NTP_PACKET_SIZE);

  memset(buff, 0, NTP_PACKET_SIZE);

  buff[0] = 0xe3;   // LI (leap indicator)
  buff[1] = 0x00;   // clock type
  buff[2] = 0x06;   // request interval
  buff[3] = 0xec;   // precission
                    // 8 zero bytes
  buff[12] = 0x31;
  buff[13] = 0x4e;
  buff[14] = 0x31;
  buff[15] = 0x34;

  return NTP_PACKET_SIZE;
}

/** Parse NTP responce. Return unix time. */
unsigned long ntp_parse_responce(const byte* buff, byte len)
{
    assert(len >= NTP_PACKET_SIZE);

    /* time mark placed in 40-43 bytes of responce */
    unsigned long highWord = word(buff[40], buff[41]);
    unsigned long lowWord = word(buff[42], buff[43]);

    /* time since 1.01.1990 in seconds */
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    DPV("NTP time  : ", secsSince1900);

    /* convert NTP time to Unix time */
    /* Unix time starts from 1.01.1970. Diference in seconds - 2208988800. */
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    DPV("Unix time : ", epoch);

    return epoch;
}

unsigned long get_utc_hours(unsigned long epoch)
{
    DPV("Unix time : ", epoch);
    DPV("The UTC hours : ", (epoch % 86400L) / 3600);

    return (epoch % 86400L) / 3600;
}

unsigned long get_utc_minutes(unsigned long epoch)
{
    DPV("Unix time : ", epoch);
    DPV("The UTC minutes : ", (epoch % 3600) / 60);

    return (epoch % 3600) / 60;
}

unsigned long get_utc_seconds(unsigned long epoch)
{
    DPV("Unix time : ", epoch);
    DPV("The UTC seconds : ", epoch % 60);

    return epoch % 60;
}

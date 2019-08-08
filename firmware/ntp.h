/* -*- mode: c -*- */

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

#ifndef __NTP_H__
#define __NTP_H__

#include "config.h"
#include "net.h"
#include "ticker.h"

#define NTP_PACKET_SIZE  48              /**< NTP packet size */

class NTP
{
private:
    unsigned long epoch;

    byte request(byte* buff, byte len);
    unsigned long response(const byte* buff, byte len);

public:
    NTP(const Ticker& ticker) : {}

    bool sync(NetServer& net);
    bool sync(NetServer& net, const char* pool);
    unsigned long get_utc_hours() const;
    unsigned long get_utc_minutes() const;
    unsigned long get_utc_seconds() const;
};

#endif /* __NTP_TIME_H__ */

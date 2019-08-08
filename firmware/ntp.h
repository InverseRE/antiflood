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

#ifndef __NTP_H__
#define __NTP_H__

#include "net.h"
#include "ticker.h"

class NTP
{
private:
    const Ticker& _ticker;
    unsigned long _epoch;

    byte make_ntp_request(byte* buff, byte len);
    unsigned long parse_ntp_response(const byte* buff, byte len);

public:
    NTP(const Ticker& ticker) : _ticker(ticker), _epoch(0) {}

    bool sync(NetServer& net, const char* pool, unsigned short port, unsigned short wait_time);
    unsigned long get_utc_epoch(bool current_time) const;
    unsigned short get_utc_hours(bool current_time) const;
    byte get_utc_minutes(bool current_time) const;
    byte get_utc_seconds(bool current_time) const;
};

#endif /* __NTP_TIME_H__ */

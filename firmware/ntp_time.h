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

#ifndef __NTP_TIME_H__
#define __NTP_TIME_H__

#include "config.h"

#define NTP_PACKET_SIZE  48              /**< NTP packet size */
#define NTP_SERVER_POOL  "pool.ntp.org"  /**< default NTP server pool */

/** Make NTP reguest. */
byte ntp_make_request(byte* buff, byte len);

/** Parse NTP responce. */
unsigned long ntp_parse_responce(const byte* buff, byte len);

unsigned long get_utc_hours(unsigned long epoch);
unsigned long get_utc_minutes(unsigned long epoch);
unsigned long get_utc_seconds(unsigned long epoch);

#endif /* __NTP_TIME_H__ */

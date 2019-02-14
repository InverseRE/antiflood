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

#ifndef __NET_H__
#define __NET_H__

#include "config.h"



#define SHIELD_PWR_SAVE_ACTION  2           /**< turn off WIFI & activate power save mode action identifier */
#define WEB_IP_STR              APP_DEFAULT_IP_S



/** Configure web-server. */
void web_configure(void);

/** Process web-server events. */
void web_run(void);

/** Stop communications. */
void web_stop(void);

/** Suspend interface. */
void web_suspend(void);

/** Return actual wifi shield state. */
bool get_wifi_state(void);



#endif  /* __NET_H__ */

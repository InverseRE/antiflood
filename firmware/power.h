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

#ifndef __POWER_H__
#define __POWER_H__

#include "config.h"



/**
 * Setup the Watch Dog Timer (WDT).
 * WDT will generate interrupt without reset in about 8 sec.
 */
void wdt_setup();

/** Enters the arduino into a sleep mode. */
void enter_sleep(boolean adc_off, boolean bod_off);

/** Apply power save mode. */
void power_save_mode_on(void);



#endif  /* __POWER_H__ */

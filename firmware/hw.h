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

#ifndef __HW_H__
#define __HW_H__

#include <avr/wdt.h>
#include "config.h"



/**
 * Configure peripherals.
 * Checks buttons pressed.
 *
 * @return true - no buttons pressed, false - some button is pressed
 */
bool hw_configure();

/** Perform reset by watchdog timer. */
void hw_reset(void);

/** Perform reset by watchdog timer. */
void hw_reset_delay(void);

/** Suspend for a while. */
void hw_suspend(unsigned long time);

/** Power down. */
void hw_sleep(void);

/** Make millis() starts from 0 again. */
void hw_reset_time(void);

/** Checks if internal power supplied. */
bool hw_power_internal(void);

/** Emulate internal-power mode. */
bool hw_power_save(void);

#endif /* __HW_H__ */

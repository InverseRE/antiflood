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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <Arduino.h>

#define SW_RX     4                       /**< software serial rx (debug printout) */
#define SW_TX     5                       /**< software serial tx (debug printout) */

/* Debug config. */
#define DEBUG_PRINTOUT                    /* debug printout */
#define DEBUG_BAUD_RATE 9600              /* software serial baud rate */

/* Init serial for debug print. */
void start_db_print(void);
/* Print string from stack. */
void DPS(String str);
void DPSLN(String str);
/* Print int from stack. */
void DPI(int x);

#endif  /* __DEBUG_H__ */

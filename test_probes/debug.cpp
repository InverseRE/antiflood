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

#include "debug.h"

#ifdef DEBUG_PRINTOUT
#include <SoftwareSerial.h>
#endif

/* Debug printout. */
#ifdef DEBUG_PRINTOUT
SoftwareSerial SWS(SW_RX, SW_TX);
void DPS(String str) { SWS.print(str); }
void DPSLN(String str) { SWS.println(str); }
void DPI(int x) { SWS.print(x); }
#else
void DPS(String str) { }
void DPSLN(String str) { }
void DPI(int x) { }
#endif

void start_db_print(void) {
#ifdef DEBUG_PRINTOUT
    SWS.begin(DEBUG_BAUD_RATE);
#endif
}
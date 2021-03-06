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

#include "debug.h"

#ifdef DEBUG

#define DEBUG_BAUD_RATE 115200              /* software serial baud rate */

SoftwareSerial SWS(SW_RX, SW_TX);

void DPI(void)
{
    SWS.begin(DEBUG_BAUD_RATE);
}

void DPC_(const __FlashStringHelper* const msg)
{
    SWS.println(msg);
}

void DPA_(const __FlashStringHelper* const msg, const byte* ptr, int cnt)
{
    byte c = cnt < 255 ? cnt : 255;
    String size(cnt);

    SWS.print(msg);
    SWS.print(F(" ["));
    SWS.print(size);
    SWS.print(F("] "));
    while (c--) {
        if (*ptr < 0x10) {
            SWS.print("0");
        }
        SWS.print(*ptr++, HEX);
    }
    SWS.println(F(""));
}

int DPM_(void)
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#endif  /* DEBUG */

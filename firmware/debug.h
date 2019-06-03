/* -*-*- mode: c++ -*-*- */

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

/* Debug config configure. */
#define DEBUG_PRINTOUT                      /* debug printout */

#ifdef DEBUG_PRINTOUT



#include <Arduino.h>

/** Debug printout initialization. */
void DPI(void);

/** Debug printout a message. */
#define DPC(msg) DPC_(F(msg))
void DPC_(const __FlashStringHelper* const msg);

/** Debug printout a variable value. */
#define DPV(msg, val) DPV_(F(msg), val)

/* XXX: separating declarations and definitions doesn't work for templates */
#include <SoftwareSerial.h>
extern SoftwareSerial SWS;
template <typename T> void DPV_(const __FlashStringHelper* const msg, const T& val)
{
    SWS.print(msg);
    SWS.print(F(": "));
    SWS.println(val);
}

/** Debug printout an array value. */
#define DPA(msg, ptr, cnt) DPA_(F(msg), (byte*)&(ptr), cnt)
void DPA_(const __FlashStringHelper* const msg, const byte* ptr, int cnt);

/** Debug print if test fails. */
#define DPT(test) { if (!(test)) { DPV("Function fails: " #test " " __FILE__, __LINE__); } }



#else  /* DEBUG_PRINTOUT */



#define DPI()
#define DPC(x)
#define DPV(x, y)
#define DPA(x, y, z)
#define DPT(x) { (x); }



#endif  /* DEBUG_PRINTOUT */

#endif  /* __DEBUG_H__ */

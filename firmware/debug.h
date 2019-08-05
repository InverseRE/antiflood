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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "config.h"




#if defined DEBUG_URGNET || defined DEBUG_INFO || defined DEBUG_DETAILS
#define DEBUG
#endif



#ifdef DEBUG

#include <Arduino.h>
#include <SoftwareSerial.h>

extern SoftwareSerial SWS;

/** Debug printout initialization. */
void DPI(void);

/** Debug printout a message. */
#define DPC(msg) DPC_(F(msg))
void DPC_(const __FlashStringHelper* const msg);

/** Debug printout a variable value. */
#define DPV(msg, val) DPV_(F(msg), val)
template <typename T> void DPV_(const __FlashStringHelper* const msg, const T& val)
{
    SWS.print(msg);
    SWS.print(F(" "));
    SWS.println(val);
}

/** Debug printout an array value. */
#define DPA(msg, ptr, cnt) DPA_(F(msg), (byte*)&(ptr), cnt)
void DPA_(const __FlashStringHelper* const msg, const byte* ptr, int cnt);

/** Debug print if test fails. */
#define DPT(test) { if (!(test)) { DPC("#" #test); } }

/** Free RAM. */
#define DPM(msg) DPV((msg), DPM_());
int DPM_(void);

#else  /* DEBUG_URGNET || DEBUG_INFO || DEBUG_DETAILS */

#define DPI()

#define uPC(x)
#define uPV(x, y)    (void)(y)
#define uPA(x, y, z) (void)(y); (void)(z)
#define uPT(x)       (x)
#define uPM
#define iPC(x)
#define iPV(x, y)    (void)(y)
#define iPA(x, y, z) (void)(y); (void)(z)
#define iPT(x)       (x)
#define iPM
#define dPC(x)
#define dPV(x, y)    (void)(y)
#define dPA(x, y, z) (void)(y); (void)(z)
#define dPT(x)       (x)
#define dPM

#endif  /* DEBUG */

#ifdef DEBUG_URGNET
#define uPC(x)       DPC(x)
#define uPV(x, y)    DPV(x, y)
#define uPA(x, y, z) DPA(x, y, z)
#define uPT(x)       DPT(x)
#define uPM          DPM
#define iPC(x)
#define iPV(x, y)    (void)(y)
#define iPA(x, y, z) (void)(y); (void)(z)
#define iPT(x)       (x)
#define iPM
#define dPC(x)
#define dPV(x, y)    (void)(y)
#define dPA(x, y, z) (void)(y); (void)(z)
#define dPT(x)       (x)
#define dPM
#endif

#ifdef DEBUG_INFO
#define uPC(x)       DPC(x)
#define uPV(x, y)    DPV(x, y)
#define uPA(x, y, z) DPA(x, y, z)
#define uPT(x)       DPT(x)
#define uPM          DPM
#define iPC(x)       DPC(x)
#define iPV(x, y)    DPV(x, y)
#define iPA(x, y, z) DPA(x, y, z)
#define iPT(x)       DPT(x)
#define iPM          DPM
#define dPC(x)
#define dPV(x, y)    (void)(y)
#define dPA(x, y, z) (void)(y); (void)(z)
#define dPT(x)       (x)
#define dPM
#endif

#ifdef DEBUG_DETAILS
#define uPC(x)       DPC(x)
#define uPV(x, y)    DPV(x, y)
#define uPA(x, y, z) DPA(x, y, z)
#define uPT(x)       DPT(x)
#define uPM          DPM
#define iPC(x)       DPC(x)
#define iPV(x, y)    DPV(x, y)
#define iPA(x, y, z) DPA(x, y, z)
#define iPT(x)       DPT(x)
#define iPM          DPM
#define dPC(x)       DPC(x)
#define dPV(x, y)    DPV(x, y)
#define dPA(x, y, z) DPA(x, y, z)
#define dPT(x)       DPT(x)
#define dPM          DPM
#endif

#endif  /* __DEBUG_H__ */

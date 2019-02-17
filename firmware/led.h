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

#ifndef __LED_H__
#define __LED_H__

#include "ticker.h"

/** LED states. */
enum LedMode {
    LED_OFF,                                /**< switched off */
    LED_SPIKE,                              /**< tiny light or rapid single flash */
    LED_BLINK,                              /**< periodic blinking */
    LED_ON,                                 /**< constantly on */
    LED_WARNING                             /**< rapidly flashes */
};

const String& to_string(LedMode mode);

/** LED. */
class Led {
private:
    const Ticker& _ticker;
    const byte _port;

public:
    Led(const Ticker& ticker, byte port);
    void lit(LedMode mode);
    void dim(void);
};

#endif  /* __LED_H__ */

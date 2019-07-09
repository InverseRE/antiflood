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

#ifndef __LED_H__
#define __LED_H__

#include "ticker.h"

/** LED states. */
enum LedMode {
    LED_OFF,                                /**< constantly off */
    LED_ON,                                 /**< constantly on */
    LED_BLINK                               /**< periodic blinking */
};

/** LED. */
class Led {
private:
    const Ticker& _ticker;
    const byte _port;
    LedMode _mode;

public:
    Led(const Ticker& ticker, byte port);
    void setup(void);

    void set(LedMode mode) { _mode = mode; }
    LedMode mode(void) const { return _mode; }
    byte lit(void) const;
};

#endif  /* __LED_H__ */

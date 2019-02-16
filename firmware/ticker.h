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

#ifndef __TICKER_H__
#define __TICKER_H__

#include <Arduino.h>

#define SPIKE_DURATION      30          /**< spike on duration, ms */
#define FLASH_DURATION      200         /**< rapid flashing on/off duration, ms */
#define BLINK_DURATION      1000        /**< blink on/off duration, ms */

/** Ticker to get various signals. */
class Ticker {
private:
    byte _sig_spike;
    byte _sig_blink;
    byte _sig_flash;

public:
    Ticker() : _sig_spike(LOW), _sig_blink(LOW), _sig_flash(LOW) {}

    void tick(void)
    {
        unsigned long tm = millis();
        _sig_spike = tm % (200 * SPIKE_DURATION) > SPIKE_DURATION ? LOW : HIGH;
        _sig_blink = tm % (  2 * BLINK_DURATION) > BLINK_DURATION ? LOW : HIGH;
        _sig_flash = tm % (  2 * FLASH_DURATION) > FLASH_DURATION ? LOW : HIGH;
    }

    byte sig_low(void) const { return LOW; }
    byte sig_high(void) const { return HIGH; }
    byte sig_spike(void) const { return _sig_spike; }
    byte sig_blink(void) const { return _sig_blink; }
    byte sig_flash(void) const { return _sig_flash; }
};

#endif  /* __TICKER_H__ */

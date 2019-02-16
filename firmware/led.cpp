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

#include <Arduino.h>
#include "led.h"

/** Configure and fast check for LEDs. */
void Led::Led(const Ticker& ticker, byte port)
         : _ticker(ticker), _port(port)
{
    pinMode(_port, OUTPUT);
    digitalWrite(_port, LOW);
}

void Led::lit(LedMode mode)
{
    byte sig =
              mode == LED_OFF     ? _ticker.sig_low()
            : mode == LED_SPIKE   ? _ticker.sig_spike()
            : mode == LED_BLINK   ? _ticker.sig_blink()
            : mode == LED_ON      ? _ticker.sig_high()
            : mode == LED_WARNING ? _ticker.sig_flash()
            :                       _ticker.sig_flash();

    digitalWrite(_port, sig);
}

void Led::dim(void)
{
    digitalWrite(_port, LOW);
}
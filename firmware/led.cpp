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

#include "led.h"
#include "probe.h"



#define LED_SPIKE_DURATION      30          /**< spike on duration, ms */
#define LED_FLASH_DURATION      200         /**< rapid flashing on/off duration, ms */
#define LED_BLINK_DURATION      1000        /**< blink on/off duration, ms */



/** Configure and fast check for LEDs. */
void leds_configure(void)
{
    int i = PROBES_CNT;

    while (i--) {
        pinMode(PROBES[i].led, OUTPUT);
        digitalWrite(PROBES[i].led, HIGH);
    }

    /* Light up for a moment. */
    delay(400);

    i = PROBES_CNT;
    while (i--) {
        digitalWrite(PROBES[i].led, LOW);
    }

    DP("leds configured");
}

/** Illuminate assigned LEDs. */
void leds_display(void)
{
    unsigned long tm = millis();
    byte SPIKE = tm % (200 * LED_SPIKE_DURATION) > LED_SPIKE_DURATION ? LOW : HIGH;
    byte BLINK = tm % (  2 * LED_BLINK_DURATION) > LED_BLINK_DURATION ? LOW : HIGH;
    byte FLASH = tm % (  2 * LED_FLASH_DURATION) > LED_FLASH_DURATION ? LOW : HIGH;

    int i = PROBES_CNT;

    while (i--) {
        byte sig =
                  PROBES[i].mode == LED_OFF     ? LOW
                : PROBES[i].mode == LED_SPIKE   ? SPIKE
                : PROBES[i].mode == LED_BLINK   ? BLINK
                : PROBES[i].mode == LED_ON      ? HIGH
                : PROBES[i].mode == LED_WARNING ? FLASH
                :                                 FLASH;

        digitalWrite(PROBES[i].led, sig);
    }
}

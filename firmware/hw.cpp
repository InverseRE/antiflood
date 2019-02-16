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

#include <avr/wdt.h>
#include <avr/power.h>

#include "hw.h"
#include "net.h"
#include "probe.h"



#define LED_HALT_DURATION  100
#define LED_SWITCH_LATENCY 500



/** Turn off unused modules at startup. */
void peripheral_configure()
{
    /* Disable not used timers. */
    power_timer1_disable();
    power_timer2_disable();

    /* Disable I2C. */
    power_twi_disable();

    /* Disable SPI. */
    power_spi_disable();
}

/** Signal an error by all means. */
void halt_on_error(void)
{
    int i = PROBES_CNT;

    DP("halt");

    web_stop();

    while (i--) {
        pinMode(PROBES[i].led, OUTPUT);
    }

    do {
        unsigned long tm = millis();
        byte sig = tm % (  2 * LED_HALT_DURATION) > LED_HALT_DURATION ? LOW : HIGH;

        i += tm % (  2 * LED_SWITCH_LATENCY) > LED_SWITCH_LATENCY ? 0 : 1;
        i %= PROBES_CNT;
        digitalWrite(PROBES[i].led, sig);

        delay(10);

    } while (true); /* here may be placed an option to unstack */
}

/** Perform reset by watchdog timer. */
void reset(void)
{
    /* Ensure WD timer is power up. */
    wdt_enable(WDTO_4S);
    halt_on_error();
}
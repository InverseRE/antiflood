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

#include <avr/power.h>
#include <avr/sleep.h>

#include "debug.h"
#include "hw.h"
#include "ticker.h"

extern volatile unsigned long timer0_millis;
static volatile bool wdt_ignore = false;
static volatile bool wdt_permanent = false;

ISR(TIMER1_OVF_vect)
{
}

ISR(WDT_vect)
{
    if (wdt_ignore) {

        /* Continue one time. */
        wdt_reset();
        wdt_ignore = false;

    } else {

        /*
         * Reset-only mode.
         * Ñlear WDIE (interrupt enable...7th from left).
         * Set WDE (reset enable...4th from left), and set delay interval.
         * Reset system in 16 ms...
         */
        MCUSR = 0;
        WDTCSR |= 0b00011000;
        WDTCSR = 0b00001000 | 0b000000;
        while (true);
    }
}

/** Perform reset by watchdog timer. */
void hw_reset(void)
{
    dPC("hw: reset");

    wdt_ignore = false;
    wdt_enable(WDTO_15MS);
    while (true);
}

/** Perform reset by watchdog timer. */
void hw_reset_delay(void)
{
    dPC("hw: reset delayed");

    wdt_ignore = false;
    wdt_permanent = true;
}

void wdt_configure(void)
{
    noInterrupts();

    /*
     * Interrupt-only mode.
     * Set WDCE (5th from left) and WDE (4th from left).
     * Set WDIE: interrupt enabled.
     * Clear WDE: reset disabled.
     * Interval (right side of bar) to 8 seconds
     */
    MCUSR = 0;
    WDTCSR |= 0b00011000;
    WDTCSR =  0b01000000 | 0b100001;

    interrupts();
    wdt_permanent = false;

    dPC("hw: wdt configured");
}

/** Turn off unused modules at startup. */
void hw_configure()
{
    interrupts();

    power_timer1_disable();
    power_timer2_disable();
    power_twi_disable();
    power_spi_disable();

    power_timer0_enable();
    power_adc_enable();
    power_usart0_enable();
    wdt_configure();

    dPC("hw: configured");
}

/**
 * Enters the arduino into a sleep mode.
 *
 * Uses IDLE mode, TIMER1(2^16).
 *
 * time = 1/16MHz * 1024 * (2^16 - preload) * 1000, ms
 * preload = 2^16 - (time * 16000 / 1024)
 */
void hw_suspend(unsigned long time)
{
    dPV("hw: suspend", time);

    while (wdt_permanent);
    wdt_reset();

    // time limits
    if (time < SUSPEND_MIN) {
        dPC("hw: skipped");
        return;
    }
    if (time > SUSPEND_MAX) {
        dPV("hw: defaults", SUSPEND_MAX);
        time = SUSPEND_MAX;
    }

    unsigned long ticks = (time * 125) / 8; // GCD(16000, 1024) = 128
    ticks = ticks < 65535 ? ticks : 65535;
    unsigned short preload = 65535 - (unsigned short)ticks;

    // before
    noInterrupts();

    power_adc_disable();
    power_timer0_disable();
    power_timer1_enable();

    TCCR1A = 0x00;
    TCCR1B = 0x05;
    TCNT1 = preload;
    TIMSK1 = 0x01;

    // TODO: for pcb-3 add PCINT (user's buttons)

    // suspend/resume point
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();

    interrupts();
    sleep_cpu();
    sleep_disable();
    noInterrupts();

    // after
    unsigned short ticks_passed = TCNT1 - preload;
    unsigned long time_passed = (unsigned long)ticks_passed * 8 / 125;
    unsigned long ms = timer0_millis;
    ms += time_passed;
    ms += 1; // overdraft for the above code
    timer0_millis = ms;

    TIMSK1 = 0x00;

    power_timer1_disable();
    power_timer0_enable();
    power_adc_enable();

    interrupts();

    // continue
    wdt_reset();

    dPV("hw: wake up", time_passed + 1);
}

/** Power down. */
void hw_sleep(void)
{
    dPC("hw: sleep");

    while (wdt_permanent);
    wdt_reset();
    wdt_ignore = true;

    // before
    noInterrupts();

    power_timer0_disable();
    power_adc_disable();
    power_usart0_disable();

    // power down
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    interrupts();
    sleep_cpu();
    sleep_disable();
    noInterrupts();

    // after
    unsigned long ms = timer0_millis;
    ms += 8000; // approximately
    ms += 1; // overdraft for the above code
    timer0_millis = ms;

    power_timer0_enable();
    power_adc_enable();
    power_usart0_enable();

    interrupts();

    // continue
    wdt_reset();
    wdt_ignore = false;

    dPC("hw: wake up");
}

/** Make millis() starts from 0 again. */
void hw_reset_time(void)
{
    dPC("hw: reset millis()");

    noInterrupts();
    timer0_millis = 0;
    interrupts();
}

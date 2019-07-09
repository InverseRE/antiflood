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

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "power.h"
#include "probe.h"
#include "net.h"


#ifndef DEBUG_PRINTOUT
/** Pin change Interrupt Service. This is executed when pin form A0 to A5 changed. */
ISR (PCINT1_vect) {
    /* Turn off WDT. */
    wdt_disable();
    /* Disable pin change interrupts for A0 to A5 */
    PCICR  &= ~bit(PCIE1);
}
#endif /* DEBUG_PRINTOUT */

/** Watchdog Interrupt Service. This is executed when watchdog timed out. */
ISR (WDT_vect) {
    /* Turn off WDT. */
    wdt_disable();
    /* Disable pin change interrupts for A0 to A5 */
    PCICR  &= ~bit(PCIE1);
}

/**
 * Setup the Watch Dog Timer (WDT).
 * WDT will generate interrupt without reset in about 8 sec.
 */
void wdt_setup()
{
    noInterrupts();

    /* Clear the reset flag on the MCUSR, the WDRF bit (bit 3). */
    MCUSR &= ~(1 << WDRF);

    /* Configure the Watchdog timer Control Register (WDTCSR). */
    WDTCSR |= (1 << WDCE) | (1 << WDE);

    /* Setting the watchdog pre-scaler value. */
    WDTCSR  = (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (1 << WDP0);

    /* Enable the WD interrupt (note: no reset). */
    WDTCSR |= _BV(WDIE);

    interrupts();
}

/** Enters the arduino into a sleep mode. */
void enter_sleep(boolean adc_off, boolean bod_off)
{
    int previousADCSRA;

    /* Setup watchdog. */
    wdt_setup();

    /* Prepare probes. */
    // TODO

    /* Disable interrupts. */
    noInterrupts();

    /* Pin change interrupt enable */
    PCMSK1 |= bit (PCINT8);  // pin A0
    PCMSK1 |= bit (PCINT9);  // pin A1
    PCMSK1 |= bit (PCINT10); // pin A2
    PCMSK1 |= bit (PCINT11); // pin A3
    PCMSK1 |= bit (PCINT12); // pin A4
    PCMSK1 |= bit (PCINT13); // pin A5
    PCIFR  |= bit (PCIF1);   // clear any outstanding interrupts
    PCICR  |= bit (PCIE1);   // enable pin change interrupts for A0 to A5

    /* Disable ADC */
    if (adc_off) {
        previousADCSRA = ADCSRA;
        ADCSRA = 0;
    }

    /* Use the power saving mode. */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    /* Disable interrupts. */
    noInterrupts();
    sleep_enable();

    /* Disable peripherals. */
    if (adc_off) {
        power_adc_disable();
    }

    /* Turn off brown-out enable in software. */
    if (bod_off) {
        MCUCR = bit(BODS) | bit(BODSE);
        MCUCR = bit(BODS);
    }

    /* Enable interrupts. */
    interrupts();

    /* Now enter sleep mode. */
    sleep_mode();

    /* The program will continue from here after the WDT timeout */

    /* First thing to do is disable sleep. */
    sleep_disable();

    /* Re-enable peripherals. */
    if (adc_off) {
        power_adc_enable();
    }

    /* Delay to settle down ADC and peripheries. */
    delay(10);

    /* Set previous ADC config. */
    ADCSRA = previousADCSRA;
}

/** Apply power save mode. */
void power_save_mode_on(void)
{
    /* TODO: engage power save */
}

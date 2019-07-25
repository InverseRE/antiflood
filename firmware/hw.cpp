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
#include <avr/power.h>

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "hw.h"
#include "ticker.h"

/** Perform reset by watchdog timer. */
void hw_reset(void)
{
    wdt_enable(WDTO_15MS);
    do { } while (true); // here may be placed an option to unstack
}

/** Turn off unused modules at startup. */
void hw_configure()
{
    wdt_disable();
    interrupts();

    power_timer1_disable();
    power_timer2_disable();
    power_twi_disable();
    power_spi_disable();

    power_timer0_enable();
    power_adc_enable();
    power_usart0_enable();
}

/**
 * Enters the arduino into an idle mode.
 */
void hw_suspend(unsigned long time)
{
    wdt_reset();

    // time limits
    if (time < SUSPEND_MIN) {
        return;
    }
    if (time > SUSPEND_MAX) {
        time = SUSPEND_MAX;
    }

    // before
    wdt_disable();

    // suspend/resume point
    unsigned long end = millis() + time;

    set_sleep_mode(SLEEP_MODE_IDLE);

    while (millis() < end) {
        cli();
        sleep_enable();
        sei();
        sleep_cpu();
        sleep_disable();
    }

    // after
    wdt_enable(ACTIVE_LIMIT);
}

// #ifndef DEBUG_PRINTOUT
// /** Pin change Interrupt Service. This is executed when pin form A0 to A5 changed. */
// ISR (PCINT1_vect) {
//     /* Turn off WDT. */
//     wdt_disable();
//     /* Disable pin change interrupts for A0 to A5 */
//     PCICR  &= ~bit(PCIE1);
// }
// #endif /* DEBUG_PRINTOUT */

// /**
//  * Setup the Watch Dog Timer (WDT).
//  * WDT will generate interrupt without reset in about 8 sec.
//  */
// void wdt_setup()
// {
//     /* Clear the reset flag on the MCUSR, the WDRF bit (bit 3). */
//     MCUSR &= ~(1 << WDRF);

//     /* Configure the Watchdog timer Control Register (WDTCSR). */
//     WDTCSR |= (1 << WDCE) | (1 << WDE);

//     /* Setting the watchdog pre-scaler value. */
//     WDTCSR  = (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (1 << WDP0);

//     /* Enable the WD interrupt (note: no reset). */
//     WDTCSR |= _BV(WDIE);
// }

// /** Enters the arduino into a sleep mode. */
// void hw_sleep(boolean adc_off, boolean bod_off)
// {
//     int previousADCSRA;

//     /* Disable interrupts. */
//     noInterrupts();

//     /* Setup watchdog. */
//     wdt_setup();

//     /* Prepare probes. */
//     // TODO

//     /* Pin change interrupt enable */
//     PCMSK1 |= bit (PCINT8);  // pin A0
//     PCMSK1 |= bit (PCINT9);  // pin A1
//     PCMSK1 |= bit (PCINT10); // pin A2
//     PCMSK1 |= bit (PCINT11); // pin A3
//     PCMSK1 |= bit (PCINT12); // pin A4
//     PCMSK1 |= bit (PCINT13); // pin A5
//     PCIFR  |= bit (PCIF1);   // clear any outstanding interrupts
//     PCICR  |= bit (PCIE1);   // enable pin change interrupts for A0 to A5

//     /* Disable ADC */
//     if (adc_off) {
//         previousADCSRA = ADCSRA;
//         ADCSRA = 0;
//     }

//     /* Use the power saving mode. */
//     set_sleep_mode(SLEEP_MODE_PWR_DOWN);

//     /* Disable interrupts. */
//     noInterrupts();
//     sleep_enable();

//     /* Disable peripherals. */
//     if (adc_off) {
//         power_adc_disable();
//     }

//     /* Turn off brown-out enable in software. */
//     if (bod_off) {
//         MCUCR = bit(BODS) | bit(BODSE);
//         MCUCR = bit(BODS);
//     }

//     /* Enable interrupts. */
//     interrupts();

//     /* Now enter sleep mode. */
//     sleep_mode();

//     /* The program will continue from here after the WDT timeout */

//     /* First thing to do is disable sleep. */
//     sleep_disable();

//     /* Re-enable peripherals. */
//     if (adc_off) {
//         power_adc_enable();
//     }

//     /* Delay to settle down ADC and peripheries. */
//     delay(10);

//     /* Set previous ADC config. */
//     ADCSRA = previousADCSRA;
// }

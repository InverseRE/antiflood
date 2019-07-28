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

extern volatile unsigned long timer0_millis;

ISR(TIMER1_OVF_vect)
{
}

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
 * Enters the arduino into a sleep mode.
 *
 * Uses IDLE mode, TIMER1(2^16).
 *
 * time = 1/16MHz * 1024 * (2^16 - preload) * 1000, ms
 * preload = 2^16 - (time * 16000 / 1024)
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

    unsigned long ticks = (time * 125) / 8; // GCD(16000, 1024) = 128
    ticks = ticks < 65535 ? ticks : 65535;
    unsigned short preload = 65535 - (unsigned short)ticks;

    // before
    noInterrupts();

    wdt_disable();
    power_timer0_disable();
    power_timer1_enable();

    TCCR1A = 0x00;
    TCCR1B = 0x05;
    TCNT1 = preload;
    TIMSK1 = 0x01;

    // suspend/resume point
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();

    interrupts();
    sleep_cpu();
    sleep_disable();
    noInterrupts();

    // correction for millis()
    unsigned short ticks_passed = TCNT1 - preload;
    unsigned long time_passed = (unsigned long)ticks_passed * 8 / 125;
    unsigned long ms = timer0_millis;

    ms += time_passed;
    ms += 1; // overdraft for the above code
    timer0_millis = ms;

    // after
    TIMSK1 = 0x00;

    power_timer1_disable();
    power_timer0_enable();
    wdt_enable(ACTIVE_LIMIT);

    interrupts();
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

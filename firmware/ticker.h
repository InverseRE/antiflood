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

#define LED_SPIKE_DURATION      30          /**< spike on duration, ms */
#define LED_FLASH_DURATION      200         /**< rapid flashing on/off duration, ms */
#define LED_BLINK_DURATION      1000        /**< blink on/off duration, ms */
#define PROBE_CHECK_DURATION    1           /**< measurement waitng delay: t = sqrt(R*C), ms */
#define VALVE_OPERATION_LIMIT   12000       /**< amount of time for valve's action, ms*/
#define VALVE_OPERATION_EXTRA   20000       /**< extra time for an ongoing action, ms*/
#define SETUP_DELAY             1000        /**< startup time for the device, ms */
#define SHIELD_STARTUP_TIME     1000        /**< startup time for ESP8266, ms */
#define SHIELD_SHUTDOWN_TIME    100         /**< shutdown time for ESP8266, ms */
#define SHIELD_TRX_LATENCY      10          /**< some delays in web communication, ms */
#define LOOP_POLLING_LATENCY    10          /**< basic loop period, ms */
#define WEB_HEADING_COUNT       5           /**< web page redirect delay, s */

/**
 * Ticker.
 * Generate signals,
 * perform timing, time intervals.
 */
class Ticker {
private:
    byte _sig_spike;
    byte _sig_blink;
    byte _sig_flash;

public:
    Ticker() : _sig_spike(LOW), _sig_blink(LOW), _sig_flash(LOW) {}
    void setup(void) {}

    unsigned long tick(void)
    {
        unsigned long tm = millis();
        _sig_spike = tm % (200 * LED_SPIKE_DURATION) > LED_SPIKE_DURATION ? LOW : HIGH;
        _sig_blink = tm % (  2 * LED_BLINK_DURATION) > LED_BLINK_DURATION ? LOW : HIGH;
        _sig_flash = tm % (  2 * LED_FLASH_DURATION) > LED_FLASH_DURATION ? LOW : HIGH;
        return tm;
    }

    unsigned long mark(void) const { return millis(); }

    byte sig_low(void) const { return LOW; }
    byte sig_high(void) const { return HIGH; }
    byte sig_spike(void) const { return _sig_spike; }
    byte sig_blink(void) const { return _sig_blink; }
    byte sig_flash(void) const { return _sig_flash; }

    void delay_setup(void) const { delay(SETUP_DELAY); }
    void delay_shield_up(void) const { delay(SHIELD_STARTUP_TIME); }
    void delay_shield_down(void) const { delay(SHIELD_SHUTDOWN_TIME); }
    void delay_shield_trx(void) const { delay(SHIELD_TRX_LATENCY); }
    void delay_probe(void) const { delay(PROBE_CHECK_DURATION); }
    void delay_loop(void) const { delay(LOOP_POLLING_LATENCY); }

    byte web_heading_count(void) const { return WEB_HEADING_COUNT; }

    bool limit_valve(unsigned long mark) {
        return mark != 0
                && millis() - mark > VALVE_OPERATION_LIMIT;
    }
};

#endif  /* __TICKER_H__ */

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

#ifndef __TICKER_H__
#define __TICKER_H__

#include <Arduino.h>

#define SUSPEND_MIN             4           /**< minimum suspend time, ms*/
#define SUSPEND_MAX             4194        /**< maximum suspend time, ms*/

#define FAR_NEXT                (-1)
#define SYNC_POSTPONE_NEXT      30000       /**< postpone sync for 30 secs */
#define SYNC_FAIL_NEXT          3600000     /**< sync over next hour if fails */
#define SYNC_NEXT               36000000    /**< sync once per 10 hours */
#define REBOOT_NEXT             1728000000  /**< make a reboot in a 20 days */
#define LED_NEXT                100         /**< blink's next schedule, ms */
#define PROBE_IDLE_NEXT         4000        /**< measurement period, ms */
#define PROBE_ACTIVE_NEXT       4000        /**< measurement period, ms */
#define LINE_IDLE_NEXT          8000        /**< measurement period, ms */
#define LINE_ACTIVE_NEXT        8000        /**< measurement period, ms */
#define VALVE_NEXT              100         /**< valve's period, ms */
#define VALVE_FORGET_NEXT       30000       /**< ignore valves known state period, ms */ // TODO: rmove

#define LED_BLINK_DURATION      1000        /**< blink on/off duration, ms */
#define PROBE_CHECK_DURATION    1           /**< measurement waitng delay: t = sqrt(R*C), ms */
#define VALVE_OPERATION_LIMIT   12000       /**< amount of time for valve's action, ms*/
#define VALVE_OPERATION_EXTRA   20000       /**< extra time for an ongoing action, ms*/
#define LOOP_POLLING_LATENCY    10          /**< basic loop period, ms */

/**
 * Ticker.
 * Generate signals,
 * perform timing, time intervals.
 */
class Ticker {
private:
    byte _sig_blink;

public:
    Ticker() : _sig_blink(LOW) {}
    void setup(void) {}

    unsigned long tick(void)
    {
        unsigned long tm = millis();
        _sig_blink = tm % ( 2 * LED_BLINK_DURATION) > LED_BLINK_DURATION ? LOW : HIGH;
        return tm;
    }

    unsigned long mark(void) const { return millis(); }

    byte sig_low(void) const { return LOW; }
    byte sig_high(void) const { return HIGH; }
    byte sig_blink(void) const { return _sig_blink; }

    void suspend(unsigned long t) const { delay(t); } // TODO: replace with a suspend function
    void delay_probe(void) const { delay(PROBE_CHECK_DURATION); }
    void delay_loop(void) const { delay(LOOP_POLLING_LATENCY); }

    bool limit_valve(unsigned long mark) const {
        return mark != 0
                && millis() - mark > VALVE_OPERATION_LIMIT;
    }
};

#endif  /* __TICKER_H__ */

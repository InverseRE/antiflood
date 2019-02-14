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

#include "valve.h"
#include "app.h"



/** List of presented valves. */
valve_t VALVES[] = {
    {VALVE_IGNORE, VALVE_IGNORE, 0, 0, VFOPST, VCONSC, OPEN, CLOSE}
};

/** Count of valves. */
int VALVES_CNT = (sizeof(VALVES) / sizeof(VALVES[0]));



#define TRIG_LVL                HIGH        /**< action engage level */
#define IDLE_LVL                LOW         /**< idle level */
#define VALVE_DURATION_AUTO     6000        /**< given time to perform an action before retry, alarm or halt */
#define VALVE_DURATION_FORCED   20000       /**< given time to perform a forced action before alarm or halt */
#define VALVE_CURRENT_MIN       5           /**< normalized value of minimal current of valves */
#define VALVE_CURRENT_MAX       25          /**< normalized value of maximum current of valves */
#define VALVE_OPENING_TIME      5000        /**< amount of time to open valve, ms*/
#define VALVE_CLOSING_TIME      6000        /**< amount of time to close valve, ms*/
#define VALVE_FORCEOPENING_TIME 10000       /**< amount of time to force valve opening, ms*/
#define VALVE_FORCECLOSING_TIME 15000       /**< amount of time to force valve closing, ms*/



/** Configure and fast check for valves. */
void valves_configure(void)
{
    int j = VALVES_CNT;

    while (j--) {
        pinMode(VALVES[j].fport, INPUT);
        pinMode(VALVES[j].vport, INPUT);
        pinMode(VALVES[j].oport, OUTPUT);
        pinMode(VALVES[j].cport, OUTPUT);

        digitalWrite(VALVES[j].oport, IDLE_LVL);
        digitalWrite(VALVES[j].cport, IDLE_LVL);
    }

    DP("valves control lines configured");
}


/** Perform an action. */
void valves_run(void)
{
    int j = VALVES_CNT;

    while (j--) {
        const valve_state_t act = VALVES[j].act_state;
        const valve_state_t exp = VALVES[j].exp_state;
        const byte open = VALVES[j].oport;
        const byte close = VALVES[j].cport;

        switch (exp) {

        case VALVE_IGNORE:
            /* Do nothing. */
            break;

        case VALVE_OPEN:
            /* If actual state should be ignored, or malfunction happened,
               or already opened then do stop
               but at first power down the other line. */
            digitalWrite(close, IDLE_LVL);
            digitalWrite(open, act == VALVE_CLOSE ? TRIG_LVL : IDLE_LVL);
            break;

        case VALVE_CLOSE:
            /* If actual state should be ignored, or malfunction happened,
               or already closed then do stop
               but at first power down the other line. */
            digitalWrite(open, IDLE_LVL);
            digitalWrite(close, act == VALVE_OPEN ? TRIG_LVL : IDLE_LVL);
            break;

        case VALVE_MALFUNCTION:
        default:
            app_set_state(APP_MALFUNCTION);
            DP("valves fail");

            /* Stop operations. */
            digitalWrite(open, IDLE_LVL);
            digitalWrite(close, IDLE_LVL);
        }
    }
}

/** Check actual status. */
void valves_check(void)
{
    int j = VALVES_CNT;

    while (j--) {
        const valve_state_t exp = VALVES[j].exp_state;
        const valve_state_t act = VALVES[j].act_state;
        const unsigned long elt = VALVES[j].elt;
        const unsigned long limit = exp == VALVE_OPEN ? VALVE_OPENING_TIME : VALVE_CLOSING_TIME;
        unsigned long tm = millis();

        /* TODO: supply current readings are unused for now */
        /* byte val = analogRead(VALVES[j].vport) >> 2; */
        /* bool is_idle = val < VALVE_CURRENT_MIN; */
        /* bool is_high = val > VALVE_CURRENT_MAX; */

        /* VALVES[j].act_state = */
        /*           is_high ? VALVE_MALFUNCTION */
        /*         : is_idle ? exp */
        /*         :           VALVE_IGNORE; */

        /* If action completed. */
        if (exp == act) {
            app_state_t app_st = app_get_state();
            app_state_t st = exp == VALVE_CLOSE ? APP_SOLVED : app_st;
            app_st = st > app_st ? st : app_st;
            app_set_state(app_st);
        }

        /* Skip completed or bad states. */
        if (exp == act || exp == VALVE_IGNORE || exp == VALVE_MALFUNCTION) {
            continue;
        }

        /* Start or limit the operation time. */
        if (elt == 0) {
            VALVES[j].act_state = exp == VALVE_OPEN ? VALVE_CLOSE : VALVE_OPEN;
            VALVES[j].elt = tm;
        } else if (tm - elt > limit) {
            VALVES[j].act_state = exp;
            VALVES[j].elt = 0;
        }
    }
}

/**
 * Check if any not complete valves actions.
 *
 * @return 1 - there is some not complete actions
 * @return 0 - there is no uncomplete actions
 */
int is_valves_actions(void)
{
    int j = VALVES_CNT;

    while (j--) {
        const valve_state_t exp = VALVES[j].exp_state;
        const valve_state_t act = VALVES[j].act_state;

        /* Skip completed or bad states. */
        if (exp == act || exp == VALVE_IGNORE || exp == VALVE_MALFUNCTION) {
            continue;
        }

        /* Valve not complete action. */
        if (exp != act) {
            return 1;
        }
    }

    return 0;
}

/** Force valves to open. */
void valves_force_open()
{
    int j = VALVES_CNT;

    while (j--) {
        VALVES[j].act_state = VALVE_IGNORE;
        VALVES[j].exp_state = VALVE_OPEN;
        VALVES[j].elt = 0;

        digitalWrite(VALVES[j].cport, IDLE_LVL);
        digitalWrite(VALVES[j].oport, TRIG_LVL);
    }

    delay(VALVE_FORCEOPENING_TIME);

    j = VALVES_CNT;
    while (j--) {
        digitalWrite(VALVES[j].oport, IDLE_LVL);

        VALVES[j].act_state = VALVE_OPEN;
        VALVES[j].exp_state = VALVE_OPEN;
    }

    if (!app_check_state(APP_MALFUNCTION)) {
        app_set_state(APP_OK);
    }
}

/** Force valves to close. */
void valves_force_close()
{
    int j = VALVES_CNT;

    while (j--) {
        VALVES[j].act_state = VALVE_IGNORE;
        VALVES[j].exp_state = VALVE_CLOSE;
        VALVES[j].elt = 0;

        digitalWrite(VALVES[j].oport, IDLE_LVL);
        digitalWrite(VALVES[j].cport, TRIG_LVL);
    }

    delay(VALVE_FORCECLOSING_TIME);

    j = VALVES_CNT;
    while (j--) {
        digitalWrite(VALVES[j].cport, IDLE_LVL);

        VALVES[j].act_state = VALVE_CLOSE;
        VALVES[j].exp_state = VALVE_CLOSE;
    }

    if (!app_check_state(APP_MALFUNCTION)) {
        app_set_state(APP_OK);
    }
}

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



#include "config.h"



/** Valve states and controls. */
typedef enum {
    VALVE_IGNORE,                           /**< valve disconnected or ignored */
    VALVE_OPEN,                             /**< 'open' state */
    VALVE_CLOSE,                            /**< 'close' state */
    VALVE_MALFUNCTION,                      /**< some malfunction detected */
} valve_state_t;

/** Valve. */
typedef struct {
    valve_state_t exp_state;                /**< valve state to get */
    valve_state_t act_state;                /**< actual valve state */
    unsigned long elt;                      /**< duration of a current action, ms */
    byte chk;                               /**< self check readings */
    byte fport;                             /**< input port for forced operation */
    byte vport;                             /**< input self check port */
    byte oport;                             /**< port to perform an 'open' action */
    byte cport;                             /**< port to perform a 'close' action */
} valve_t;



/** List of presented valves. */
static valve_t VALVES[] = {
    {VALVE_IGNORE, VALVE_IGNORE, 0, 0, VFOPST, VCONSC, OPEN, CLOSE}
};

/** Count of valves. */
int VALVES_CNT = (sizeof(VALVES) / sizeof(VALVES[0]));



/** Configure and fast check for valves. */
static void valves_configure(void)
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
static void valves_run(void)
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
            OVERALL_STATE = APP_MALFUNCTION;
            DP("valves fail");

            /* Stop operations. */
            digitalWrite(open, IDLE_LVL);
            digitalWrite(close, IDLE_LVL);
        }
    }
}

/** Check actual status. */
static void valves_check(void)
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
            app_state_t app_st = exp == VALVE_CLOSE ? APP_SOLVED : OVERALL_STATE;
            OVERALL_STATE = app_st > OVERALL_STATE ? app_st : OVERALL_STATE;
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
static int is_valves_actions(void)
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
static void valves_force_open()
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

    OVERALL_STATE = APP_MALFUNCTION != OVERALL_STATE ? APP_OK : OVERALL_STATE;
}

/** Force valves to close. */
static void valves_force_close()
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

    OVERALL_STATE = APP_MALFUNCTION != OVERALL_STATE ? APP_OK : OVERALL_STATE;
}

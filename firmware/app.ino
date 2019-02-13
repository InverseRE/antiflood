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



/** Overall detector states. */
typedef enum {
    APP_OK = 0, /**< all is good */
    APP_ALARM = 1, /**< water detected */
    APP_SOLVED = 2, /**< valves closed */
    APP_MALFUNCTION = 3 /**< something is wrong */
} app_state_t;

/** Application overall state. */
static app_state_t OVERALL_STATE = APP_MALFUNCTION;



/** Choose the next move. */
static void app_solve(void)
{
    valve_state_t gst = VALVE_IGNORE;

    int i = PROBES_CNT;
    int j = VALVES_CNT;

    while (i--) {
        const probe_live_state_t cst = PROBES[i].con;
        const probe_detector_state_t dst = PROBES[i].det;
        const valve_state_t vst =
                  cst == PROBE_ERROR   ? VALVE_CLOSE
                : dst == PROBE_WATER   ? VALVE_CLOSE
                : cst == PROBE_ONLINE  ? VALVE_IGNORE
                : dst == PROBE_DRY     ? VALVE_IGNORE
                : cst == PROBE_OFFLINE ? VALVE_IGNORE
                :                        VALVE_CLOSE;

        gst = gst == VALVE_IGNORE ? vst : gst;
    }

    while (j--) {
        VALVES[j].exp_state = gst == VALVE_IGNORE ? VALVES[j].exp_state : gst;
    }
}

/** Check if app state queals to given state. */
static bool app_check_state(app_state_t state)
{
    return OVERALL_STATE == state;
}

/** Check if app state queals to given state. */
static void app_set_state(app_state_t state)
{
    OVERALL_STATE == state;
}

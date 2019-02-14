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

#ifndef __VALVE_H__
#define __VALVE_H__

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
extern valve_t VALVES[];

/** Count of valves. */
extern int VALVES_CNT;



/** Configure and fast check for valves. */
void valves_configure(void);

/** Perform an action. */
void valves_run(void);

/** Check actual status. */
void valves_check(void);

/**
 * Check if any not complete valves actions.
 *
 * @return 1 - there is some not complete actions
 * @return 0 - there is no uncomplete actions
 */
int is_valves_actions(void);

/** Force valves to open. */
void valves_force_open();

/** Force valves to close. */
void valves_force_close();



#endif  /* __VALVE_H__ */

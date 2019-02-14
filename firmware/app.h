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



/** Choose the next move. */
void app_solve(void);

/** Check if app state queals to given state. */
bool app_check_state(app_state_t state);

/** Set app state. */
void app_set_state(app_state_t state);

/** Get app state. */
app_state_t app_get_state(void);

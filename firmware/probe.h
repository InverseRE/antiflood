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

#ifndef __PROBE_H__
#define __PROBE_H__

#include "config.h"



/** Probe sensor states. */
typedef enum {
    PROBE_DRY,                              /**< nothing happens, all is OK */
    PROBE_WATER                             /**< some activity detected */
} probe_detector_state_t;

/** Probe connection status. */
typedef enum {
    PROBE_OFFLINE,                          /**< disconnected or non-controlled probe */
    PROBE_ONLINE,                           /**< probe is connected */
    PROBE_ERROR                             /**< shortcircuit, malfunction or bad readings */
} probe_live_state_t;

/** Probe. */
typedef struct {
    byte port;                              /**< analog input port */
    byte val;                               /**< readed static value */
    byte chk;                               /**< readed dynamic value (connection check) */
    byte elt;                               /**< elapsed time (connection check), normalized */
    probe_detector_state_t det;             /**< probe detector state */
    probe_live_state_t con;                 /**< probe connection state */
    byte led;                               /**< linked LED */
    led_state_t mode;                       /**< linked LED's status */
} probe_t;



/** List of available probes. */
extern probe_t PROBES[];

/** Count of probes. */
extern  int PROBES_CNT;



/** Configure and fast check for probes. */
void probes_configure(void);

/** Perform readings from probes. */
void probes_test(void);

/** Check connection status for probes. */
void probes_check(void);

/** Calculate probes' states. */
void probes_result(void);



#endif  /* __PROBE_H__ */

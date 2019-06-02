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

#include "debug.h"
#include "probe.h"

/** List of available probes. */
int PROBES[] = {PROBE0, PROBE1, PROBE2, PROBE3, PROBE4, PROBE5};

/** Count of probes. */
const int PROBES_CNT = (sizeof(PROBES) / sizeof(PROBES[0]));

int val[PROBES_CNT] = {0};

/** Configure probes. */
void probes_configure(void)
{
    int i = PROBES_CNT;

    while (i--) {
        pinMode(PROBES[i], INPUT_PULLUP);
    }
}

/** Perform readings from probes. */
void probes_test(void)
{
    int i = PROBES_CNT;

    while (i--) {
        val[i] = analogRead(PROBES[i]);
        delay(2);
    }

    i = PROBES_CNT;

    while (i--) {
        DPI(val[i]);
        DPS(" ");
    }

    DPSLN("");
    delay(10);
}

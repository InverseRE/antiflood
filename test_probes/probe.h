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

#include <Arduino.h>

#define PROBE0    A0                        /**< probe 0 */
#define PROBE1    A1                        /**< probe 1 */
#define PROBE2    A2                        /**< probe 2 */
#define PROBE3    A3                        /**< probe 3 */
#define PROBE4    A4                        /**< probe 4 */
#define PROBE5    A5                        /**< probe 5 */

/** Configure probes. */
void probes_configure(void);

/** Perform readings from probes. */
void probes_test(void);

#endif  /* __PROBE_H__ */

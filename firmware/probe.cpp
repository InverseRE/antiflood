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

#include <Arduino.h>
#include "probe.h"

#define PROBE_V_SHORT_CIRCUIT   5           /**< lower value is treated as short circuit */
#define PROBE_V_FLOOD_TRIGGER   175         /**< lower value is treated like a signal */
#define CG_MIN                  10          /**< minimal voltage on capasitor, normalized */
#define CG_MAX_FACTOR           0.8         /**< factor of maximal voltage on capasitor */

Probe::Probe(const Ticker& ticker, byte port)
        : _ticker(ticker), _port(port)
{
    _time_mark = 0;
    _sensor = PROBE_DRY;
    _connection = PROBE_OFFLINE;

    pinMode(_port, OUTPUT);
    digitalWrite(_port, LOW);
}

void Probe::prepare(void)
{
    _sensor = PROBE_UNKNOWN;

    /* should be LOW for the moment */
    _time_mark = _ticker.mark();
    pinMode(_port, INPUT_PULLUP);
}

void Probe::delay(void) const
{
    _ticker.delay_probe();
}

ProbeConnection Probe::test_connection(void)
{
    unsigned long time = _ticker.mark();
    unsigned long elt = (time - _time_mark) / 4;
    elt = elt == 0 ? 1 : elt;
    elt = elt < 255 ? elt : 255;

    byte v = analogRead(_port) >> 2;

    _connection =
              v < PROBE_V_SHORT_CIRCUIT ? PROBE_ERROR
            : t == 0                    ? PROBE_OFFLINE
            : c > v * CG_MAX_FACTOR     ? PROBE_OFFLINE
            : c > CG_MIN                ? PROBE_ONLINE
            :                             PROBE_ERROR;

    return _connection;
}

ProbeSensor Probe::test_sensor(void)
{
    bool triggered = (analogRead(_port) >> 2) < PROBE_V_FLOOD_TRIGGER;

    pinMode(_port, OUTPUT);
    digitalWrite(_port, LOW);

    _sensor = triggered ? PROBE_WATER : PROBE_DRY;

    return _sensor;
}

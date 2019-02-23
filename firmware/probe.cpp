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
    _value = 0;
    _time_mark = 0;
    _sensor = PROBE_DRY;
    _connection = PROBE_OFFLINE;
}

void Probe::setup(void)
{
    pinMode(_port, INPUT_PULLUP);
}

ProbeSensor Probe::test_sensor(void)
{
    pinMode(_port, INPUT_PULLUP);
    _value = analogRead(_port) >> 2;
    _sensor = _value < PROBE_V_FLOOD_TRIGGER ? PROBE_WATER : PROBE_DRY;

    pinMode(_port, OUTPUT);
    digitalWrite(_port, LOW);
    _time_mark = _ticker.mark();

    return _sensor;
}

void Probe::delay(void) const
{
    _ticker.delay_probe();
}

ProbeConnection Probe::test_connection(void)
{
    pinMode(_port, INPUT_PULLUP);

    if (_sensor == PROBE_WATER) {
        return _connection = PROBE_ONLINE;
    }

    unsigned long t = (_ticker.mark() - _time_mark) / 4;
    t = t < 255 ? t : 255;
    byte c = analogRead(_port) >> 2;

    _connection =
              c < PROBE_V_SHORT_CIRCUIT  ? PROBE_ERROR
            : t == 255                   ? PROBE_OFFLINE
            : c > _value * CG_MAX_FACTOR ? PROBE_OFFLINE
            : c > CG_MIN                 ? PROBE_ONLINE
            :                              PROBE_ERROR;

    return _connection;
}

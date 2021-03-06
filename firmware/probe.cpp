/* -*-*- mode: c++ -*-*- */

/*
   Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko

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

#include "debug.h"
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

    dPV("prb: setup", _port);
}

ProbeSensor Probe::test_sensor(void)
{
    if (_is_sens_ovr) {
        dPV("prb: test ovr", _ovr_sens);
        _is_sens_ovr = false;
        return _sensor = _ovr_sens;
    }

    if (_time_limit > _ticker.mark()) {
        iPC("prb: test time limit");
        return _sensor = PROBE_UNAWARE;
    }

    pinMode(_port, INPUT_PULLUP);
    _value = analogRead(_port) >> 2;

    pinMode(_port, OUTPUT);
    digitalWrite(_port, LOW);

    _sensor = _value < PROBE_V_FLOOD_TRIGGER ? PROBE_WATER : PROBE_DRY;
    _time_mark = _ticker.mark();
    pinMode(_port, INPUT_PULLUP);

    if (_sensor == PROBE_WATER) {
        dPV("prb: sig", _value);
        dPV("prb: water port", _port);
    }

    return _sensor;
}

void Probe::delay(void) const
{
    _ticker.delay_probe();
}

ProbeConnection Probe::test_connection(void)
{
    if (_is_conn_ovr) {
        dPV("prb: conn ovr", _ovr_conn);
        _is_conn_ovr = false;
        return _connection = _ovr_conn;
    }

    if (_time_limit > _ticker.mark()) {
        iPC("prb: conn time limit");
        return _connection = PROBE_OFFLINE;
    }

    pinMode(_port, INPUT_PULLUP);

    unsigned long t = (_ticker.mark() - _time_mark) / 4;
    t = t < 255 ? t : 255;
    byte c = analogRead(_port) >> 2;

    _connection =
              c < PROBE_V_SHORT_CIRCUIT  ? PROBE_ERROR
            : t == 255                   ? PROBE_OFFLINE
            : c > _value * CG_MAX_FACTOR ? PROBE_OFFLINE
            : c > CG_MIN                 ? PROBE_ONLINE
            :                              PROBE_ERROR;

    if (_connection == PROBE_ERROR) {
        dPV("prb: sig", c);
        dPV("prb: error port", _port);
    }

    return _connection;
}

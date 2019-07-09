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

#ifndef __PROBE_H__
#define __PROBE_H__

#include "ticker.h"

/** Probe sensor states. */
enum ProbeSensor {
    PROBE_DRY,                              /**< nothing happens, all is OK */
    PROBE_UNAWARE,                          /**< ignored */
    PROBE_WATER                             /**< some activity detected */
};

/** Probe connection status. */
enum ProbeConnection{
    PROBE_OFFLINE,                          /**< disconnected or non-controlled probe */
    PROBE_ONLINE,                           /**< probe is connected */
    PROBE_ERROR                             /**< shortcircuit, malfunction or bad readings */
};

/**
 * Probe.
 *
 * Sequence follows:
 * .test_sensor [i]
 * .delay (only once)
 * .test_link [i]
 * loop again with some delay to discharge capacitor
 */
class Probe {
private:
    const Ticker& _ticker;
    const byte _port;                       /**< analog input port */
    byte _value;                            /**< normalized sensor value */
    unsigned long _time_mark;               /**< an engage time mark */
    ProbeSensor _sensor;                    /**< probe's detector state */
    ProbeConnection _connection;            /**< probe's connection state */
    unsigned long _time_limit;              /**< action/state end time, ms */

    bool _is_sens_ovr;                      /**< is sensor value overrided? (once) */
    bool _is_conn_ovr;                      /**< is connection value overrided? (once) */
    ProbeSensor _ovr_sens;                  /**< emulated detector state */
    ProbeConnection _ovr_conn;              /**< emulated connection state */

public:
    Probe(const Ticker& ticker, byte port);
    void setup(void);

    ProbeSensor test_sensor(void);
    void delay(void) const;
    ProbeConnection test_connection(void);

    unsigned long time_limit(void) const { return _time_limit; }
    void enable(void) { _time_limit = _ticker.mark(); }
    void disable(unsigned long duration) { _time_limit = _ticker.mark() + duration; }
    ProbeConnection connection(void) const { return _connection; }
    ProbeSensor sensor(void) const { return _sensor; }

    void emulate_water(void) {
        _is_sens_ovr = true;
        _ovr_sens = PROBE_WATER;
    }

    void emulate_error(void) {
        _is_conn_ovr = true;
        _ovr_conn = PROBE_ERROR;
    }
};

#endif  /* __PROBE_H__ */

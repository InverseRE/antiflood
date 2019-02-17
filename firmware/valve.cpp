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
#include "app.h"

#define TRIG_LVL                HIGH        /**< action engage level */
#define IDLE_LVL                LOW         /**< idle level */
#define VALVE_DURATION_AUTO     6000        /**< given time to perform an action before retry, alarm or halt */
#define VALVE_DURATION_FORCED   20000       /**< given time to perform a forced action before alarm or halt */
#define VALVE_CURRENT_MIN       5           /**< normalized value of minimal current of valves */
#define VALVE_CURRENT_MAX       25          /**< normalized value of maximum current of valves */
#define VALVE_OPENING_TIME      5000        /**< amount of time to open valve, ms*/
#define VALVE_CLOSING_TIME      6000        /**< amount of time to close valve, ms*/
#define VALVE_FORCEOPENING_TIME 10000       /**< amount of time to force valve opening, ms*/
#define VALVE_FORCECLOSING_TIME 15000       /**< amount of time to force valve closing, ms*/

Valve::Valve(const Ticker& ticker,
        byte verif_switch_port, byte verif_supply_port,
        byte engage_open_port, byte engage_close_port)
        : _ticker(ticker),
          _vport_switch(verif_switch_port), _vport_supply(verif_supply_port),
          _oport(engage_open_port), _cport(engage_close_port)
{
    _exp_state = VALVE_IGNORE;
    _act_state = VALVE_IGNORE;
    _ovr_state = VALVE_IGNORE;

    pinMode(_vport_switch, INPUT);
    pinMode(_vport_supply, INPUT);
    pinMode(_oport, OUTPUT);
    pinMode(_cport, OUTPUT);
    digitalWrite(_oport, IDLE_LVL);
    digitalWrite(_cport, IDLE_LVL);

    _time_mark = 0;
}

bool Valve::is_engaged(void) const
{

    /* Skip bad states. */
    if (_exp_state == VALVE_IGNORE || _exp_state == VALVE_MALFUNCTION) {
        return false;
    }

    /* Valve has a uncompleted action. */
    return _exp_state != _act_state;
}

bool Valve::open(void)
{
    if (_ovr_state != VALVE_IGNORE) {
        return false;
    }

    _exp_state = VALVE_OPEN;

    return true;
}

void Valve::close(void)
{
    if (_ovr_state != VALVE_IGNORE) {
        return false;
    }

    _exp_state = VALVE_CLOSE;

    return true;
}

bool Valve::force_open(void)
{
    if (_ovr_state != VALVE_IGNORE) {
        return false;
    }

    _ovr_state = VALVE_OPEN;

    return true;
}

void Valve::force_close(void)
{
    if (_ovr_state != VALVE_IGNORE) {
        return false;
    }

    _ovr_state = VALVE_CLOSE;

    return true;
}

void Valve::run(void)
{
    /* TODO: read a hand-override signal to protect a hardware */
    // if (...) {
    //     _ovr_state = ...;
    //     _time_mark = 0; /* reset timer */
    // }

    /* Overrided action? */
    if (_ovr_state != VALVE_IGNORE) {
        _exp_state = _ovr_state;
    }

    /* An action start-mark. */
    if (_act_state != _exp_state && _time_mark != 0) {
        _time_mark = _ticker.mark();
    }

    /* Engage controls. */
    switch (_exp_state) {

    case VALVE_IGNORE:
        /* Do nothing. */
        _time_mark = 0;
        break;

    case VALVE_OPEN:
        /* If actual state should be ignored, or malfunction happened,
           or already opened then do stop
           but at first power down the other line. */
        digitalWrite(_cport, IDLE_LVL);
        digitalWrite(_oport, _act_state != VALVE_OPEN ? TRIG_LVL : IDLE_LVL);
        break;

    case VALVE_CLOSE:
        /* If actual state should be ignored, or malfunction happened,
           or already closed then do stop
           but at first power down the other line. */
        digitalWrite(_oport, IDLE_LVL);
        digitalWrite(_cport, _act_state != VALVE_CLOSE ? TRIG_LVL : IDLE_LVL);
        break;

    case VALVE_MALFUNCTION:
    default:
        /* Stop operations. */
        digitalWrite(_oport, IDLE_LVL);
        digitalWrite(_cport, IDLE_LVL);
        _act_state = VALVE_MALFUNCTION;
        _exp_state = VALVE_IGNORE;
    }

    /* TODO: read a supply current to adjust _time_mark and/or _act_state */
    // {
    //     _act_state = ...;
    //     _time_mark = ...;
    // }

    /* Time limits. */
    if (_ticker.limit_valve(_time_mark)) {
        _act_state = _exp_state;
        _time_mark = 0;
    }

    return _act_state;
}

/* -*-*- mode: c++ -*-*- */

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
#include "debug.h"

#define TRIG_LVL                HIGH        /**< action engage level */
#define IDLE_LVL                LOW         /**< idle level */
#define ACTIVE_OP_TRIGGER       2           /**< minimum supply readings for an active op */
#define MANUAL_OP_TRIGGER       800         /**< some signal is active */
#define MANUAL_OP_TRIGGER_BOTH  50          /**< both signals are active */

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
    _time_mark = 0;
    _extra_time = 0;
    _is_ovr = false;
}

void Valve::setup(void)
{
    /* pinMode() should be ommited for A6/A7*/
    pinMode(_oport, OUTPUT);
    pinMode(_cport, OUTPUT);
    digitalWrite(_oport, IDLE_LVL);
    digitalWrite(_cport, IDLE_LVL);
    _is_ovr = false;
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

bool Valve::is_overrided(void) const
{
    return _is_ovr;
}

bool Valve::open(void)
{
    if (_is_ovr) {
        return false;
    }

    _exp_state = VALVE_OPEN;

    return true;
}

bool Valve::close(void)
{
    if (_is_ovr) {
        return false;
    }

    _exp_state = VALVE_CLOSE;

    return true;
}

bool Valve::force_open(void)
{
    if (_is_ovr) {
        return false;
    }

    _ovr_state = VALVE_OPEN;

    return true;
}

bool Valve::force_close(void)
{
    if (_is_ovr) {
        return false;
    }

    _ovr_state = VALVE_CLOSE;

    return true;
}

ValveState Valve::run(void)
{
    /* Take into account a manualy overridden signals. */
    int switch_readings = analogRead(_vport_switch);

    /* TODO: an actual vavle's state: by switch and supply current */
    /* TODO: signal to App actual state */

    if (switch_readings < MANUAL_OP_TRIGGER_BOTH) {
        /* stop all operations, due to a conflict */
        DPV("valve conflicts, switch readings", switch_readings);
        digitalWrite(_oport, IDLE_LVL);
        digitalWrite(_cport, IDLE_LVL);
        _act_state = VALVE_IGNORE;
        _exp_state = VALVE_IGNORE;
        _ovr_state = VALVE_IGNORE;
        _time_mark = 0;
        _is_ovr = true;
    } else if (switch_readings < MANUAL_OP_TRIGGER && _is_ovr) {
        DPV("valve overrided, switch readings", switch_readings);
        _act_state = VALVE_IGNORE;
        _exp_state = VALVE_IGNORE;
        _ovr_state = VALVE_IGNORE;
        _time_mark = 0;
    } else {
        _is_ovr = false;
    }

    /* Overrided action? */
    if (_ovr_state != VALVE_IGNORE) {
        _exp_state = _ovr_state;
        _ovr_state = VALVE_IGNORE;
        _time_mark = 0;
    }

    /* An action start-mark. */
    if (_act_state != _exp_state && _time_mark == 0) {

        /* Operation starts. */
        DPC("valve engaged");
        _time_mark = _ticker.mark();
        _extra_time = VALVE_OPERATION_EXTRA;
    }

    /* Engage controls. */
    switch (_exp_state) {

    case VALVE_IGNORE:
        /* Do nothing. */
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
        /* Operation aborts. */
        DPC("valve malfunction");
        digitalWrite(_oport, IDLE_LVL);
        digitalWrite(_cport, IDLE_LVL);
        _act_state = VALVE_MALFUNCTION;
        _exp_state = VALVE_IGNORE;
        _ovr_state = VALVE_IGNORE;
        _time_mark = 0;
    }

    /* Time limits. */
    if (_ticker.limit_valve(_time_mark)) {
        int supply_readings = analogRead(_vport_supply);
        if (supply_readings > ACTIVE_OP_TRIGGER
                && _extra_time > 0) {

            /* Extra time needed? (apply only once) */
            DPV("valve ops granted an extra time, supply readings", supply_readings);
            _time_mark += _extra_time;
            _extra_time = 0;

        } else {

            /* Operation stops. */
            DPV("valve disengaged by timeout, supply readings", supply_readings);
            digitalWrite(_oport, IDLE_LVL);
            digitalWrite(_cport, IDLE_LVL);
            _act_state = _exp_state;
            _exp_state = VALVE_IGNORE;
            _ovr_state = VALVE_IGNORE;
            _time_mark = 0;
        }
    }

    return _act_state;
}

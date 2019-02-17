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

#ifndef __VALVE_H__
#define __VALVE_H__

#include "ticker.h"

/** Valve states and controls. */
enum ValveState {
    VALVE_IGNORE,                           /**< valve disconnected or ignored */
    VALVE_OPEN,                             /**< 'open' state */
    VALVE_CLOSE,                            /**< 'close' state */
    VALVE_MALFUNCTION,                      /**< some malfunction detected */
};

const String& to_string(ValveState state);

/** Valve. */
class Valve {
private:
    const Ticker& _ticker;
    const byte _vport_switch;               /**< input self check port (voltage on the switch) */
    const byte _vport_supply;               /**< input self check port (supply current) */
    const byte _oport;                      /**< output port to perform an 'open' action */
    const byte _cport;                      /**< output port to perform a 'close' action */
    ValveState _exp_state;                  /**< valve state to get */
    ValveState _act_state;                  /**< actual valve state */
    ValveState _ovr_state;                  /**< overrided valve state */
    unsigned long _time_mark;               /**< start of a current action, ms */

public:
    Valve(const Ticker& ticker,
            byte verif_switch_port, byte verif_supply_port,
            byte engage_open_port, byte engage_close_port);

    bool is_engaged(void) const;
    bool open(void);
    bool close(void);
    bool force_open(void);
    bool force_close(void);
    ValveState run(void);                   /* returns an actual state */

    ValveState state_actual(void) const { return _act_state; }
    ValveState state_expected(void) const { return _exp_state; }
    ValveState state_overrided(void) const { return _ovr_state; }
};

#endif  /* __VALVE_H__ */

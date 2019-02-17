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

#ifndef __APP_H__
#define __APP_H__

#include "ticker.h"
#include "led.h"
#include "probe.h"
#include "valve.h"

/** Overall states. */
enum AppState {
    APP_OK,                                 /**< all is good */
    APP_ALARM,                              /**< water detected */
    APP_SOLVED,                             /**< valves closed */
    APP_STANDBY,                            /**< controls override */
    APP_MALFUNCTION                         /**< something is wrong */
};

const String& to_string(AppState state);

/** Basic logic. */
class App {
private:
    const Ticker& _ticker;
    Led* const _leds;
    Probe* const _probes;
    Valve* const _valves;
    const byte _leds_cnt;
    const byte _probes_cnt;
    const byte _valves_cnt;
    AppState _state;

public:
    App(const Ticker& ticker,
            Led* leds, byte leds_cnt,
            Probe* probes, byte probes_cnt,
            Valve* valves, byte valves_cnt);

    AppState solve(void);
};

#endif  /* __APP_H__ */

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

#include "app.h"
#include "config.h"

App::App(const Ticker& ticker,
        Led* leds, byte leds_cnt,
        Probe* probes, byte probes_cnt,
        Valve* valves, byte valves_cnt)
        : _ticker(ticker),
          _leds(leds), _probes(probes), _valves(valves),
          _leds_cnt(leds_cnt), _probes_cnt(probes_cnt), _valves_cnt(valves_cnt)
{
    _state = _probes_cnt == _leds_cnt && _probes_cnt > 0
            ? APP_OK : APP_MALFUNCTION;
}

void App::setup(void)
{
}

AppState App::solve(void)
{
    /* in case of hard error */
    if (_state == APP_MALFUNCTION) {
        DPC("App::solve() APP_MALFUNCTION");
        for (int i = 0; i < _leds_cnt; ++i) {
            _leds[i].lit(LED_WARNING);
        }
        return _state;
    }

    /* read data */
    bool is_triggered = false;
    bool is_resolved = false;
    bool is_overrided = false;

    for (int i = 0; i < _probes_cnt; ++i) {
        is_triggered |= PROBE_WATER == _probes[i].test_sensor();
    }

    _probes[0].delay();

    for (int i = 0; i < _probes_cnt; ++i) {
        is_triggered |= PROBE_ERROR == _probes[i].test_connection();
    }

    /* resolve */
    for (int i = 0; is_triggered && i < _valves_cnt; ++i) {
        _valves[i].close();
    }
    for (int i = 0; i < _valves_cnt; ++i) {
        is_resolved |= VALVE_CLOSE == _valves[i].run();
        is_overrided |= VALVE_IGNORE != _valves[i].state_override();
    }

    /* summary */
    for (int i = 0; i < _probes_cnt && i < _leds_cnt; ++i) {
        switch (_probes[i].connection()) {
        case PROBE_OFFLINE: _leds[i].lit(LED_OFF);     break;
        case PROBE_ONLINE:  _leds[i].lit(LED_SPIKE);   break;
        case PROBE_ERROR:   _leds[i].lit(LED_WARNING); break;
        }

        switch (_probes[i].sensor()) {
        case PROBE_DRY:                                break;
        case PROBE_WATER:   _leds[i].lit(LED_BLINK);   break;
        }
    }

    if (is_overrided) {
        _state = APP_STANDBY;
    } else if (!is_triggered && !is_resolved) {
        _state = APP_OK;
    } else if (is_triggered && !is_resolved) {
        _state = APP_ALARM;
    } else if (!is_triggered && is_resolved) {
        _state = APP_OK;
    } else {
        _state = APP_SOLVED;
    }

    return _state;
}

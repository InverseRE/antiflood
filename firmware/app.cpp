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
        DPC("app: malfunction");
        for (int i = 0; i < _leds_cnt; ++i) {
            _leds[i].set(LED_BLINK);
        }
        return _state;
    }

    /* read data */
    bool is_triggered = false;
    bool is_engaged = false;
    bool is_overrided = false;

    for (int i = 0; i < _probes_cnt; ++i) {
        is_triggered |= PROBE_WATER == _probes[i].sensor();
    }

    for (int i = 0; i < _probes_cnt; ++i) {
        is_triggered |= PROBE_ERROR == _probes[i].connection();
    }

    /* summary */
    for (int i = 0; is_triggered && i < _valves_cnt; ++i) {
        is_overrided |= !_valves[i].close();
        is_engaged |= _valves[i].is_engaged();
        is_overrided |= _valves[i].is_overrided();
    }

    for (int i = 0; i < _probes_cnt && i < _leds_cnt; ++i) {
        switch (_probes[i].connection()) {
        case PROBE_OFFLINE: _leds[i].set(LED_OFF);     break;
        case PROBE_ONLINE:  _leds[i].set(LED_OFF);     break;
        case PROBE_ERROR:   _leds[i].set(LED_BLINK);   continue;
        }
        switch (_probes[i].sensor()) {
        case PROBE_DRY:                                break;
        case PROBE_UNAWARE:                            break;
        case PROBE_WATER:   _leds[i].set(LED_BLINK);   break;
        }
    }

    if (is_overrided) {
        _state = APP_STANDBY;
    } else if (!is_triggered && !is_engaged) {
        _state = APP_OK;
    } else if (is_triggered && !is_engaged) {
        _state = APP_ALARM;
    } else if (!is_triggered && is_engaged) {
        _state = APP_OK;
    } else {
        _state = APP_SOLVED;
    }

    return _state;
}

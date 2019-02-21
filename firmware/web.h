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

#ifndef __WEB_H__
#define __WEB_H__

#include <WiFiEspClient.h>
#include "ticker.h"
#include "app.h"

/** User commands. */
enum WebAction {
    WEB_UNKNOWN,                            /* unknown */
    WEB_NOT_FOUND,                          /* not found */
    WEB_STATE,                              /* show state */
    WEB_OPEN,                               /* open valves */
    WEB_CLOSE,                              /* close valves */
    WEB_SUSPEND                             /* enter a power-save mode */
};

/** HTML page constructor. */
class WebPage {
private:
    const Ticker& _ticker;
    const WiFiEspClient& _client;

public:
    WebPage(const Ticker& ticker, const WiFiEspClient& client);
    void setup(void);

    WebAction parse(const String& request);
    void response_state(AppState app_state,
            const Led* leds, byte leds_cnt,
            const Probe* probes, byte probes_cnt,
            const Valve* valves, byte valves_cnt);
    void response_not_found(void);
    void heading(WebAction action, byte count_down);
};

#endif  /* __WEB_H__ */

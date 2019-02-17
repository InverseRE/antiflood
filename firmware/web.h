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
#include "timer.h"

/** User commands. */
enum WebAction {
    WEB_UNKNOWN,                            /* unknown */
    WEB_STATE,                              /* show state */
    WEB_OPEN,                               /* open valves */
    WEB_CLOSE,                              /* close valves */
    WEB_SUSPEND                             /* enter a power-save mode */
};

class WebPage {
private:
    const WiFiEspClient& _client;

public:
    WebPage(const WiFiEspClient& client);

    WebAction parse(const String& request);
    void response(void);
    void response_not_found(void);
    void heading(WebAction action);
}

/** Response for valves open/close action with redirection to the main page. */
void http_action_response(WiFiEspClient client, unsigned char act, unsigned char count_down);

/** Construct an HTML-page of an actual status. */
void http_response(WiFiEspClient client);

/** Simple response for favicon request from browser. */
void http_response_not_found(WiFiEspClient client);



#endif  /* __WEB_H__ */

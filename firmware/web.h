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

#include <WiFiEspClient.h>



/** Checks client request. */
void http_parse_request(WiFiEspClient client, const String& rbuff);

/** Response for valves open/close action with redirection to the main page. */
void http_action_response(WiFiEspClient client, unsigned char act, unsigned char timeout);

/** Construct an HTML-page of an actual status. */
void http_response(WiFiEspClient client);

/** Simple response for favicon request from browser. */
void http_response_not_found(WiFiEspClient client);

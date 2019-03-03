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
#include "proto.h"

ProtoSession::ProtoSession(const Ticker& ticker, const NetServer& server)
        : _ticker(ticker), _server(server)
{
}

void ProtoSession::setup(void)
{
}

ProtoAction ProtoSession::inform(
        AppState app_state,
        const Led* leds, byte leds_cnt,
        const Probe* probes, byte probes_cnt,
        const Valve* valves, byte valves_cnt)
{
    ProtoAction action = PROTO_UNKNOWN;

    while (_server.available()) {
        byte buf[255];
        int len;

        /* TODO: parse packet */
        /* len = _server.read(buf, sizeof(buf)); */

        /* TODO: switch action */
        /* action = ...; */

        /* TODO: response */
        /* _server.write(...); */
        /* _server.tx(); */
    }


    return action;
}

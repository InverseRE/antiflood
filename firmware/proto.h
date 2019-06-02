/* -*-*- mode: c++ -*-*- */

/*
   Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko

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

#ifndef __PROTO_H__
#define __PROTO_H__

#include "ticker.h"
#include "app.h"
#include "net.h"

/** User commands. */
enum ProtoAction {
    PROTO_UNKNOWN,                            /* unknown */
    PROTO_STATE,                              /* show state */
    PROTO_OPEN,                               /* open valves */
    PROTO_CLOSE,                              /* close valves */
    PROTO_SUSPEND,                            /* enter a power-save mode */
    PROTO_EN_PROBE,                           /* enable probes */
    PROTO_DIS_PROBE,                          /* disable probes */
    PROTO_GET_SETTING,                        /* read setting */
    PROTO_SET_SETTING,                        /* append/update setting */
    PROTO_EMU_WATER,                          /* emulate WATER on probes */
    PROTO_EMU_ERROR                           /* emulate ERROR on probes */
};

/** Client session. */
class ProtoSession {
private:
    const Ticker& _ticker;
    NetServer& _server;

public:
    ProtoSession(const Ticker& ticker, NetServer& server);
    void setup(void);

    ProtoAction action(
            byte (*state)(byte* buf, byte buf_max_size),
            bool (*open)(void),
            bool (*close)(void),
            bool (*suspend)(void),
            bool (*enable)(byte idx),
            bool (*disable)(byte idx, unsigned long duration),
            int (*get_setting)(byte type, byte* buff, byte* size),
            int (*set_setting)(byte type, const byte* data, byte size),
            bool (*emu_water)(byte idx, bool immediately),
            bool (*emu_error)(byte idx, bool immediately));
};

#endif  /* __PROTO_H__ */

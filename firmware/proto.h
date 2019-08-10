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

/** Client session. */
class ProtoSession {
private:
    const Ticker& _ticker;
    NetServer& _server;

public:
    ProtoSession(const Ticker& ticker, NetServer& server);
    void setup(void);

    void action(
            unsigned long (*time)(bool sync, bool ref, bool raw),
            byte (*state)(byte* buf, byte buf_max_size),
            bool (*open)(void),
            bool (*close)(void),
            bool (*suspend)(void),
            bool (*enable)(byte idx),
            bool (*disable)(byte idx, unsigned long duration),
            bool (*emu_water)(byte idx, bool immediately),
            bool (*emu_error)(byte idx, bool immediately),
            byte (*get_wifi)(byte* buf, byte buf_max_size),
            bool (*set_wifi)(const byte* buf, byte buf_size),
            bool (*set_wifi_pwd)(const byte* buf, byte buf_size),
            byte (*get_serv)(byte* buf, byte buf_max_size),
            bool (*set_serv)(const byte* buf, byte buf_size),
            byte (*get_ntp)(byte* buf, byte buf_max_size),
            bool (*set_ntp)(const byte* buf, byte buf_size),
            bool (*settings_load)(bool def),
            bool (*settings_save)(void));
};

#endif  /* __PROTO_H__ */

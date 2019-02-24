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

#ifndef __SETTING_H__
#define __SETTING_H__

#include <Arduino.h>

/** Error codes. */
enum SettingErrorCodes {
    OK = 0,
    CRC_ERROR = 1,
    BAD_ARGUMENT = 2
};

/** Settings */
class Setting {
private:
    byte *data;          /* payload */
    byte _len;           /* payload len */
    byte _type;          /* setting type */
    Setting::~Setting(); 

public:
    Setting();
    Setting(byte type, byte len, byte value[]);
    static unsigned short crc16(byte *block, unsigned short len);
    int push(int addr);
    int pop(int addr);
    short Setting::length(void);
};

#endif  /* __SETTING_H__ */
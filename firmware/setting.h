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
#include "debug.h"

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

public:

    Setting();
    Setting(byte type, byte len, byte value[]);
    Setting::~Setting(); 

    int push(int addr);
    int pop(int addr);
    
    /* crc-16 calculation */
    static unsigned short crc16(byte *block, unsigned short len);
       
    /* get total length in bytes */   
    short Setting::length(void) const 
    {
        return _len + 4;
    }
    
    /* get setting type */
    byte Setting::get_type() const
    {
        return _type;   
    }
    
    /* get payload length */
    byte Setting::get_len() const
    {
        return _len;   
    }
    
    /* get payload */
    void Setting::get_data(byte *value) const
    {
        memcpy(value, data, _len);
    }
};

#endif  /* __SETTING_H__ */
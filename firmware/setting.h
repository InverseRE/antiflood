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

/** Storage record type. */
#define STORAGE_TYPE   0

/** Error codes. */
enum SettingErrorCodes {
    OK = 0,
    CRC_ERROR = 1,
    BAD_ARGUMENT = 2
};

/** Setting types. */
enum SettingTypes {
    WIFI_MODE = 1,
    IP_ADDRESS = 2,
    PORT_NUMBER = 3,
    SSID = 4,
    PASSWORD = 5,
    CHANNEL = 6,
    AUTH_TYPE = 7
};

/** Settings */
class Setting {
private:
    byte *data;          /* payload */
    byte _len;           /* payload len */
    byte _type;          /* setting type */
    unsigned short _crc; /* check sum */

public:

    Setting();
    Setting(byte type, byte len, const byte *value);
    Setting::~Setting(); 

    int push(int addr);
    int pop(int addr);
    
    /* crc-16 calculation */
    static unsigned short crc16(const byte *block, unsigned short len);
       
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
    
    /* get heck sum */
    byte Setting::get_crc() const
    {
        return _crc;   
    }
    
    /* get payload */
    byte Setting::get_data(byte *buff, byte buff_len = 255) const
    {
        byte len = buff_len < _len ? buff_len : _len;
        memcpy(buff, data, len);
        return len;
    }
    
    /* check equality */
    bool operator == (const Setting &Ref) const 
    {
        return ((this->_type == Ref._type) &&
                (this->_len == Ref._len) &&
                (this->_crc == Ref._crc));
    }
};

#endif  /* __SETTING_H__ */

/* -*- mode: c++ -*- */

/*
   Antiflood Copyright (C) 2019 Alexey <SmallShark> Khomyakovsky
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

#ifndef __SETTING_H__
#define __SETTING_H__

#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

#define EEPROM_SETTING_ADDRESS 0

static const byte PROGMEM dscrc_table[] = {
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
    190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
    219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
    87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

struct __attribute__((packed)) Setting {
    byte wifi_mode;
    byte wifi_ch;
    byte wifi_auth;
    char wifi_ssid[48];
    char wifi_pwd[48];
    byte serv_addr[4];
    unsigned short serv_port;
    char ntp_host[56];

    void defaults()
    {
        byte ip[] = {APP_DEFAULT_IP};
        String id = WIFI_DEFAULT_SSID;
        String pd = WIFI_DEFAULT_PASS;
        String hp = NTP_DEFAULT_POOL;

        wifi_mode = WIFI_DEFAULT_MODE;
        wifi_ch = WIFI_DEFAULT_CHAN;
        wifi_auth = WIFI_DEFAULT_SECU;
        id.getBytes(wifi_ssid, sizeof(wifi_ssid));
        pd.getBytes(wifi_pwd, sizeof(wifi_pwd));
        memcpy(serv_addr, ip, sizeof(serv_addr));
        serv_port = APP_DEFAULT_PORT;
        hp.getBytes(ntp_host, sizeof(ntp_host));
    }

    byte crc8() const
    {
        const byte* p = (const byte*)this;
        byte len = sizeof(*this);
        byte crc = 0;

        while (len--) {
            crc = pgm_read_byte(dscrc_table + (crc ^ *p++));
        }
        return crc;
    }

    void save() const
    {
        byte crc = crc8();
        EEPROM.put(EEPROM_SETTING_ADDRESS, *this);
        EEPROM.put(EEPROM_SETTING_ADDRESS + sizeof(*this), crc);
    }

    bool load()
    {
        EEPROM.get(EEPROM_SETTING_ADDRESS, *this);
        return crc8() == EEPROM[EEPROM_SETTING_ADDRESS + sizeof(*this)];
    }
};


#endif  /* __SETTING_H__ */

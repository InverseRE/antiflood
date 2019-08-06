/* -*- mode: c++ -*- */

/*
   Antiflood Copyright (C) 2019 Alexey <SmallShark> Khomyakovsky

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

#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <Arduino.h>
#include "debug.h"
#include "setting.h"

/* storage raw data size */
#define RAW_SIZE  (sizeof(store_t))

/** Error codes. */
enum StorageErrorCodes {
    ST_OK = 0,
    ST_READ_ERROR = 10,
    ST_WRITE_ERROR = 20,
    ST_BAD_ARGUMENT = 30,
    ST_NOT_FOUND = 40,
    ST_NOT_ENAUGHT = 50
};

/* storage structure */
typedef struct store {
    byte type;
    byte len;
    byte num;
    byte dummy;
} store_t;

typedef struct storage {
    union {
        store_t str;           /* structured data */
        byte raw[RAW_SIZE];    /* raw data        */
    } data;
    unsigned short crc;        /* check sum       */
} storage_t;

class SettingsStorage {

public:
    static SettingsStorage& get_instance() {
        static SettingsStorage s;
        return s;
    }

    int load(void);
    int create(void);
    bool exist(void);

    short enumerate(void);
    int get(byte type, Setting&, short* idx=nullptr);
    int read(short idx, Setting&);
    int update(short idx, const Setting&);
    int append(const Setting&);
    int erase(short idx);
    int erase_all(bool fast=true);

    void eeprom_dp(void);

private:
    storage_t st;
    int st_addr = 0;

    SettingsStorage() {}
    ~SettingsStorage() {}
    SettingsStorage(SettingsStorage const&) = delete;
    SettingsStorage& operator= (SettingsStorage const&) = delete;

    int pop(void);
    int push(int addr);
};

template <typename T>
void load_setting(SettingsStorage& sst, byte type, T& st)
{
    Setting s;
    if (ST_OK == sst.get(type, s) && s.get_len() == sizeof(T)) {
        s.get_data((byte*)&st);
    }
}

void load_setting_bytes(SettingsStorage& sst, byte type, byte* st);
void load_setting_string(SettingsStorage& sst, byte type, String& st);

int read_setting(SettingsStorage& sst, byte type, byte* buff, byte* len);
int write_setting(SettingsStorage& sst, byte type, const byte* data, byte len);

#endif  /* __STORAGE_H__ */

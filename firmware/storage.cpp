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

#include <EEPROM.h>
#include "storage.h"

/* check storage equality. */
bool st_equals(storage_t a, storage_t b) {
    return (a.crc == b.crc) && (0 == memcmp(a.data.raw, b.data.raw, sizeof(a.data.raw)));
}

/* check storage record. */
bool is_storage(storage_t a) {
    return ((STORAGE_TYPE == a.data.str.type) &&
            (2 == a.data.str.len) &&
            (0 == a.data.str.dummy) &&
            (a.crc == Setting::crc16(a.data.raw, sizeof(a.data.raw))));
}

/* get address by index. */
int get_addr(short idx) {
    int addr = 0;
    for (int i = 0; i < idx && addr < EEPROM.length(); i++) {
        byte type = EEPROM.read(addr);
        byte len = EEPROM.read(addr + 1);
        if (type && len) {
            addr += len + 4;
        } else {
            return EEPROM.length();
        }
    }

    return addr;
}

/* move part of EEPROM data. */
int move(int addr, int off, int len) {
    if (!off) {
        return ST_OK;
    }
    if (addr + off + len > EEPROM.length()) {
        return ST_NOT_ENAUGHT;
    }
    for (int i = 0; i < len; i++) {
        EEPROM.update(addr + off + i, EEPROM[addr + i]);
    }

    return ST_OK;
}

/* search for storage record. */
int search() {
    int addr;
    storage_t storage;

    for (addr = 0; addr < EEPROM.length(); addr++) {
        EEPROM.get(addr, storage);
        if (is_storage(storage)) {
            return addr;
        }
    }

    return addr;
}

/* check storage exist. */
bool SettingsStorage::exist(void)
{
    return (search() != EEPROM.length());
}

/* load storage. */
int SettingsStorage::load(void)
{
    return pop();
}

/* create new storage. */
int SettingsStorage::create(void)
{
    st.data.str.type = STORAGE_TYPE;
    st.data.str.len = 2;
    st.data.str.num = 0;
    st.data.str.dummy = 0;
    st.crc = Setting::crc16(st.data.raw, sizeof(st.data.raw));
    return push(0);
}

/* read storage data. */
int SettingsStorage::pop(void)
{
    int addr = search();
    if (addr == EEPROM.length()) {
        return ST_READ_ERROR;
    }

    EEPROM.get(addr, st);
    st_addr = addr;

    return ST_OK;
}

/* write storage data. */
int SettingsStorage::push(int addr)
{
    storage_t act;

    EEPROM.put(addr, st);
    EEPROM.get(addr, act);

    if (st_equals(act, st) != true) {
        return ST_WRITE_ERROR;
    }

    st_addr = addr;

    return ST_OK;
}

/* return number of records in storage */
short SettingsStorage::enumerate(void)
{
    Setting s;
    byte num = 0;
    int addr = 0;
    for (int i = 0; addr < st_addr; i++) {
        if (s.pop(addr) == ST_OK) {
            if (s.get_type()) {
                num++;
            } else {
                break;
            }
        }
        addr += s.length();
    }

    if (num != st.data.str.num) {
        st.data.str.num = num;
        push(st_addr);
    }

    return st.data.str.num;
}

/* get setting by index
   usage:
       SettingsStorage& sst = SettingsStorage::get_instance();
       Setting st;
       sst.read(idx, st);
*/
int SettingsStorage::read(short idx, Setting& s)
{
    if (idx < 0 || idx >= st.data.str.num) {
        return ST_BAD_ARGUMENT;
    }

    int addr = get_addr(idx);
    if (addr == EEPROM.length() || addr >= st_addr) {
        return ST_NOT_FOUND;
    }

    return s.pop(addr);
}

/* get setting by type
   usage:
       SettingsStorage& sst = SettingsStorage::get_instance();
       Setting st;
       sst.get(type, st);
*/
int SettingsStorage::get(byte type, Setting& s, short* idx = nullptr)
{
    int ret;
    const byte num = st.data.str.num;

    if (!num) {
        return ST_NOT_FOUND;
    }

    for (short i = 0; i < num; i++) {
        ret = SettingsStorage::read(i, s);
        if (ST_OK == ret && s.get_type() == type) {
            if (idx) {
                *idx = i;
            }
            return ST_OK;
        }
    }

    return ST_NOT_FOUND;
}


/* update setting by index
   usage:
       SettingsStorage& sst = SettingsStorage::get_instance();
       sst.update(idx, Setting(2, 4, "test"));
*/
int SettingsStorage::update(short idx, const Setting &s)
{
    int ret;
    Setting old;

    if (idx < 0 || idx >= st.data.str.num) {
        return ST_BAD_ARGUMENT;
    }

    int addr = get_addr(idx);
    if (addr == EEPROM.length() || addr > st_addr) {
        return ST_NOT_FOUND;
    }

    if ((ret = old.pop(addr)) != ST_OK) {
        return ret;
    }

    int off = s.length() - old.length();
    int len = st_addr - (addr + old.length()) + sizeof(storage_t);

    if (off) {
        if ((ret = move(addr + s.length(), off, len)) != ST_OK) {
            return ret;
        }
        st_addr += off;
        return s.push(addr);
    }

    return s.push(addr);
}

/* append new setting to store
   usage:
       SettingsStorage& sst = SettingsStorage::get_instance();
       sst.append(Setting(1, 6, "123456"));
*/
int SettingsStorage::append(const Setting &s)
{
    int ret;
    if (st_addr + sizeof(storage_t) + s.length() > EEPROM.length()) {
        return ST_NOT_ENAUGHT;
    }

    st.data.str.num += 1;
    st.crc = Setting::crc16(st.data.raw, sizeof(st.data.raw));
    st_addr += s.length();

    if ((ret = push(st_addr)) != ST_OK) {
        return ret;
    }

    return s.push(st_addr - s.length());
}

/* delete setting from store
   usage:
       SettingsStorage& sst = SettingsStorage::get_instance();
       sst.erase(idx);
*/
int SettingsStorage::erase(short idx)
{
    int ret;
    Setting s;

    if (idx < 0 || idx >= st.data.str.num) {
        return ST_BAD_ARGUMENT;
    }

    int addr = get_addr(idx);
    if (addr == EEPROM.length() || addr >= st_addr) {
        return ST_NOT_FOUND;
    }

    if ((ret = s.pop(addr)) != ST_OK) {
        return ret;
    }

    int off = -(s.length());
    int len = st_addr - (addr + s.length());

    if ((ret = move(addr + s.length(), off, len)) != ST_OK) {
        return ret;
    }

    st.data.str.num -= 1;
    st.crc = Setting::crc16(st.data.raw, sizeof(st.data.raw));
    st_addr -= s.length();

    return push(st_addr);
}

/* erase all record from storage */
int SettingsStorage::erase_all(bool fast = true)
{
    if (!fast) {
        for (int i = 0; i < EEPROM.length(); i++) {
            EEPROM.update(i, 0xff);
        }
    }

    return create();
}

/* print all eeprom bytes */
void SettingsStorage::eeprom_dp(void)
{
    for (int i = 0; i < EEPROM.length(); i++) {
        SWS.print(EEPROM[i]);
        SWS.print(" ");
        if (i != 0 && i % 16 == 0) {
            SWS.println("");
        }
    }
}

void load_setting_bytes(SettingsStorage& sst, byte type, byte* st)
{
    Setting s;
    if (ST_OK == sst.get(type, s)) {
        s.get_data(st);
    }
}

void load_setting_string(SettingsStorage& sst, byte type, String& st)
{
    Setting s;
    char value[256] = {0};
    if (ST_OK == sst.get(type, s)) {
        s.get_data(value);
        st = String(value);
    }
}

int read_setting(SettingsStorage& sst, byte type, byte* buff, byte* len)
{
    int ret;
    Setting s;

    if ((ret = sst.get(type, s)) == ST_OK) {
        *len = s.get_data(buff, *len);
        return s.get_len() == *len ? ST_OK : ST_NOT_ENAUGHT;
    }

    return ret;
}

int write_setting(SettingsStorage& sst, byte type, const byte* data, byte len)
{
    int ret;
    short idx;
    Setting s;

    if ((ret = sst.get(type, s, &idx)) == ST_OK) {
        return sst.update(idx, Setting(type, len, data));
    } else {
        return sst.append(Setting(type, len, data));
    }

    return ret;
}

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
            
#include <EEPROM.h>
#include "storage.h"

#define EMPTY_ADDR  0xffff

bool st_equals(storage_t a, storage_t b) {
    return (a.crc == b.crc) && (0 == memcmp(a.data.raw, b.data.raw, sizeof(a.data.raw)));
}

/* check storage exist. */
bool SettingsStorage::exist() {
    return (ST_OK == pop(false));
}

/* load storage. */
int SettingsStorage::load() {
    return pop(true);
}

/* create new storage. */
int SettingsStorage::create() {
    for (int i = 0; i < MAX_SETTIGS_NUMBER; i++)
        st.data.str.address[i] = EMPTY_ADDR;
    st.data.str.number = 0;
    st.data.str.next_free = sizeof(st);
    st.crc = Setting::crc16(st.data.raw, sizeof(st.data.raw));
    return push();
}

/* read storage data. */    
int SettingsStorage::pop(bool load) {
    storage_t s;
    EEPROM.get(0, s);
    unsigned short crc = Setting::crc16(s.data.raw, sizeof(s.data.raw));
    if (s.crc != crc) {
        return ST_READ_ERROR;
    }
    if (load) {
        st = s;
    }
    
    return ST_OK;
}

/* write storage data. */
int SettingsStorage::push(void) {
    storage_t act;
    EEPROM.put(0, st);
    EEPROM.get(0, act);
    if (st_equals(act, st) != true) {
        return ST_WRITE_ERROR;
    }

    return ST_OK;
}

/* return number of records in storage */
short SettingsStorage::enumerate(void)
{
    return st.data.str.number;
}

/* get setting by index
   usage: 
       SettingsStorage& sst = SettingsStorage::get_instance();
       Setting st = Setting();
       Setting& pst = st;
       sst.read(idx, pst);
*/
int SettingsStorage::read(short idx, Setting& s)
{
    if (idx < 0 || idx > st.data.str.number)
        return ST_BAD_ARGUMENT;
    int addr = st.data.str.address[idx];
    return s.pop(addr);
}

/* update setting by index
   usage: 
       SettingsStorage& sst = SettingsStorage::get_instance();
       sst.update(idx, Setting(2, 4, "test"));
*/
int SettingsStorage::update(short idx, Setting s)
{
    if (idx < 0 || idx > st.data.str.number)
        return ST_BAD_ARGUMENT;
    int addr = st.data.str.address[idx];
    return s.push(addr);
}

/* append new setting to store
   usage: 
       SettingsStorage& sst = SettingsStorage::get_instance();
       sst.append(Setting(1, 6, "123456"));
*/
int SettingsStorage::append(Setting s)
{
    int idx = -1;

    for (int i = 0; i < MAX_SETTIGS_NUMBER; i++) {
        if (st.data.str.address[i] == EMPTY_ADDR) {
            idx = i;
            break;
        }
    }
    
    if (idx >= 0 && idx < MAX_SETTIGS_NUMBER) {
        if (s.push(st.data.str.next_free) != ST_OK) {
            return ST_WRITE_ERROR;
        } else {   
            st.data.str.address[idx] = st.data.str.next_free;
            st.data.str.next_free += s.length();
            st.data.str.number += 1;
            st.crc = Setting::crc16(st.data.raw, sizeof(st.data.raw));;
            return push();
        }
    }
    
    return ST_FULL;
}

/* erase storage */
/* just overwrite storage header */
int SettingsStorage::erase_all(void)
{
    return create();
}

/* print all eeprom bytes */
void SettingsStorage::eeprom_dp(void)
{
    for (int i = 0; i < EEPROM.length(); i++) {
       DPS(String(EEPROM[i]));
    }
}

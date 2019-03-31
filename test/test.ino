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

#include "debug.h"
#include "setting.h"
#include "storage.h"

#define assert(val)             \
{                               \
    if (!(val)) {               \
        DP(F("false!"));        \
        DPS(String(__LINE__));  \
        while(1);               \
    }                           \
}

#define DO(call)                          \
{                                         \
    int _ret_ = (call);                   \
    if (_ret_ != ST_OK) {                 \
        DPS("RET : " + String(_ret_));    \
        DPS(String(__func__));            \
        DPS(String(__LINE__));            \
        while(1);                         \
    }                                     \
}

int counter = 0;
const int test_num = 12;
const int repeat = 1;

SettingsStorage& sst = SettingsStorage::get_instance();

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

/** Startup procedure. */
void setup() {
    start_db_print();
    //DO(sst.erase_all());
    DP(F("setup"));

    if (sst.exist()) {
        DP(F("storage already exist"));
        if (sst.load() != ST_OK) {
            DP(F("storage loading failed"));
        } else {
            DP(F("storage loaded successfully"));
            //sst.eeprom_dp();
        }
    } else {
        DP(F("storage not exist"));
        if (sst.create() != ST_OK) {
            DP(F("create failed"));
        } else {
            DP(F("storage created successfully"));
            //sst.eeprom_dp();
        }
    }

    DP(F("ready to go..."));
}

/** Main procedure. */
void loop() {
    DPS("test # " + String(counter));
    switch (counter % test_num) {
    case 0:
        test_clear_all();
        DP(F("====="));
        break;
    case 1:
        test_store_first();
        DP(F("====="));
        break;
    case 2:
        test_store_second();
        DP(F("====="));
        break;
    case 3:
        test_store_multiple();
        DP(F("====="));
        break;
    case 4:
        test_erase();
        DP(F("====="));
        break;
    case 5:
        test_read();
        DP(F("====="));
        break;
    case 6:
        test_append_new();
        DP(F("====="));
        break;
    case 7:
        test_update_same();
        DP(F("====="));
        break;
    case 8:
        test_update_lower();
        DP(F("====="));
        break;
    case 9:
        test_update_higher();
        DP(F("====="));
        break;
    case 10:
        test_clear();
        test_append_max_cnt();
        DP(F("====="));
        break;
    case 11:
        test_clear_all();
        test_append_max_size();
        DP(F("====="));
        break;
    }
    counter++;
    DPS("free memory : " + String(freeMemory()));
    
    if (counter > repeat * test_num) {
        DPS("All tests done.");
        while(1);
    }
}


void test_store_first()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 0);
    
    Setting st0 = Setting(1, 6, "123456");
    DO(sst.append(st0));
    
    num = sst.enumerate();
    assert(num == 1);
    
    Setting st;
    DO(sst.read(0, st));
    assert(st0 == st);
    
    DP(F("ok"));
}

void test_store_second()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 1);
    
    Setting st1 = Setting(2, 4, "test");
    DO(sst.append(st1));
    
    num = sst.enumerate();
    assert(num == 2);
    
    Setting st;
    DO(sst.read(1, st));
    assert(st1 == st);
    
    DP(F("ok"));
}

void test_store_multiple()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 2);
     
    Setting st3 = Setting(3, 1, "9");
    DO(sst.append(st3));
 
    Setting st4 = Setting(4, 3, "yes");
    DO(sst.append(st4));
    
    Setting st5 = Setting(5, 8, "87654321");
    DO(sst.append(st5));
    
    num = sst.enumerate();
    assert(num == 5);

    DP(F("ok"));
}

void test_erase()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 5);

    DO(sst.erase(1));
    
    num = sst.enumerate();
    assert(num == 4);
    
    DO(sst.erase(2));
    
    num = sst.enumerate();
    assert(num == 3);
    
    DP(F("ok"));
}

void test_read()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 3);

    Setting st0 = Setting(1, 6, "123456");
    Setting st1 = Setting(3, 1, "9");
    Setting st2 = Setting(5, 8, "87654321");
    
    Setting st;
    
    DO(sst.read(0, st));
    assert(st0 == st);
    
    DO(sst.read(1, st));
    assert(st1 == st);
    
    DO(sst.read(2, st));
    assert(st2 == st);
    
    DP(F("ok"));
}

void test_append_new()
{
    DPS(String(__func__));
    Setting st3 = Setting(2, 5, "test2");
    Setting st4 = Setting(4, 7, "7777777");
    
    DO(sst.append(st3));
    DO(sst.append(st4));
    
    short num = sst.enumerate();
    assert(num == 5);
    
    Setting st;
    
    DO(sst.read(3, st));
    assert(st3 == st);
    
    DO(sst.read(4, st));
    assert(st4 == st);

    DP(F("ok"));
}

void test_update_same()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 5);
    
    Setting st_old = Setting(1, 6, "123456");
    Setting st_new = Setting(11, 6, "654321");
    
    Setting st;
    
    DO(sst.read(0, st));
    assert(st_old == st);
    
    DO(sst.update(0, st_new));
    
    DO(sst.read(0, st));
    assert(st_new == st);
    
    DP(F("ok")); 
}

void test_update_lower()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 5);
    
    Setting st_old = Setting(5, 8, "87654321");
    Setting st_new = Setting(55, 2, "55");
    
    Setting st;
    
    DO(sst.read(2, st));
    assert(st_old == st);
    
    DO(sst.update(2, st_new));
    
    DO(sst.read(2, st));
    assert(st_new == st);
    
    DP(F("ok"));
}

void test_update_higher()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 5);
    
    Setting st_old = Setting(3, 1, "9");
    Setting st_new = Setting(33, 33, "this is a very long test message!");
    
    Setting st;
    
    DO(sst.read(1, st));
    assert(st_old == st);
    
    DO(sst.update(1, st_new));
    
    DO(sst.read(1, st));
    assert(st_new == st);
    
    DP(F("ok"));
}

void test_append_max_cnt()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 0);
    
    int ret = ST_OK;
    int add = 1;
    while (ret == ST_OK) {
        ret = sst.append(Setting(add++, 1, "0")); 
    }
    
    num = sst.enumerate();
    assert(num == add - 2);
    
    DP(F("ok"));
}

void test_append_max_size()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    assert(num == 0);
    
    int ret = ST_OK;
    byte data[255] = {0};
    DO(sst.append(Setting(1, sizeof(data), data)));
    DO(sst.append(Setting(2, sizeof(data), data)));
    DO(sst.append(Setting(3, sizeof(data), data)));
    assert(ST_NOT_ENAUGHT == sst.append(Setting(4, 238, data)));
    DO(sst.append(Setting(3, 237, data)));
    
    num = sst.enumerate();
    assert(num == 4);
    
    DP(F("ok"));
}

void test_clear()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    
    while(num--) {
        DO(sst.erase(num));
    }
    
    num = sst.enumerate();
    assert(num == 0);
    
    DP(F("ok"));
}

void test_clear_all()
{
    DPS(String(__func__));
    short num = sst.enumerate();
    
    sst.erase_all();
    num = sst.enumerate();
    assert(num == 0);
    
    DP(F("ok"));
}


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

#include "config.h"
#include "app.h"
#include "hw.h"
#include "led.h"
#include "net.h"
#include "power.h"
#include "probe.h"
#include "valve.h"
#include "setting.h"
#include "storage.h"



/** Startup procedure. */
void setup() {
    start_db_print();
    DP(F("setup"));

    peripheral_configure();
    web_configure();
    probes_configure();
    leds_configure();
    valves_configure();

    app_set_state(APP_OK);
    
    SettingsStorage& sst = SettingsStorage::get_instance();
    if (sst.exist()) {
        DP(F("storage already exist"));
        if (sst.load() != ST_OK) {
            DP(F("storage loading failed"));
        } else {
            DP(F("storage loaded successfully"));
        }
    } else {
        DP(F("storage not exist"));
        if (sst.create() != ST_OK) {
            DP(F("create failed"));
        } else {
            DP(F("storage created successfully"));
        }
    }

    DP(F("ready to go..."));
}

/** Main procedure. */
void loop() {
    /* Application should restart once per month.
     Check this out. */
    unsigned long tm = millis() + millis_in_sleep();
    if (tm & 0x80000000) {
        DP(F("application life time expired"));
        web_run();
        reset();
        return;
    }

    /* Check detectors. */
    probes_test();
    probes_check();
    probes_result();

    /* Control valves. */
    app_solve();

    valves_run();
    valves_check();

    /* Display info. */
    leds_display();
    if (get_wifi_state()) {
        web_run();
    }

#ifdef USE_POWER_SAVE
    /* Power save. */
    if (!get_wifi_state() && !is_valves_actions() && app_check_state(APP_OK)) {
        enter_sleep(true, true);
    } else {
        delay(10);
    }
#else
    /* Debug stub. */
    delay(10);
#endif

}

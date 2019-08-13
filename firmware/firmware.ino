/* -*-*- mode: c++ -*-*- */

/*
   Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko
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

#include "config.h"
#include "debug.h"
#include "app.h"
#include "hw.h"
#include "led.h"
#include "net.h"
#include "ntp.h"
#include "probe.h"
#include "proto.h"
#include "setting.h"
#include "scheduler.h"
#include "ticker.h"
#include "valve.h"

static Ticker ticker;

static Led leds[] = {
    {ticker, LED5},
    {ticker, LED4},
    {ticker, LED3},
    {ticker, LED2},
    {ticker, LED1},
    {ticker, LED0}};

static const byte leds_cnt = sizeof(leds) / sizeof(leds[0]);

static Probe probes[] = {
    {ticker, PROBE0},
    {ticker, PROBE1},
    {ticker, PROBE2},
    {ticker, PROBE3},
    {ticker, PROBE4},
    {ticker, PROBE5}};

static const byte probes_cnt = sizeof(probes) / sizeof(probes[0]);

static Valve valves[] = {
    {ticker, VFOPST, VCONSC, VOPEN, VCLOSE}};

static const byte valves_cnt = sizeof(valves) / sizeof(valves[0]);

static App app(ticker,
        leds, leds_cnt,
        probes, probes_cnt,
        valves, valves_cnt);

static Setting setting;

static Net* p_net = nullptr;

static NTP ntp(ticker);

static Scheduler scheduler(ticker);

static unsigned long act_time(bool sync, bool ref, bool raw);
static byte act_state(byte* buf, byte buf_max_size);
static bool act_open(void);
static bool act_close(void);
static bool act_suspend(void);
static bool act_enable(byte idx);
static bool act_disable(byte idx, unsigned long duration);
static bool act_emu_water(byte idx, bool immediately);
static bool act_emu_error(byte idx, bool immediately);
static byte act_get_wifi(byte* buf, byte buf_max_size);
static bool act_set_wifi(const byte* buf, byte buf_size);
static bool act_set_wifi_pwd(const byte* buf, byte buf_size);
static byte act_get_serv(byte* buf, byte buf_max_size);
static bool act_set_serv(const byte* buf, byte buf_size);
static byte act_get_ntp(byte* buf, byte buf_max_size);
static bool act_set_ntp(const byte* buf, byte buf_size);
static bool act_settings_load(bool def);
static bool act_settings_save(void);
static bool act_reboot(bool def_next);
static unsigned long task_net(unsigned long dt);
static unsigned long task_application(unsigned long dt);
static unsigned long task_sensors(unsigned long dt);
static unsigned long task_connections(unsigned long dt);
static unsigned long task_display(unsigned long dt);
static unsigned long task_valves(unsigned long dt);
static unsigned long task_reboot(unsigned long dt);
static unsigned long task_sync(unsigned long dt);

/** Startup procedure. */
void setup()
{
    /* Debug printout. */
    DPI();
    iPC("start");

    /* Setup HW devices */
    bool normal_boot = hw_configure();

    /* Settings. */
    if (normal_boot && setting.load()) {
        iPC("custom config");
    } else {
        iPC("default config");
        setting.defaults();
    }

    dPV("wifi mode", setting.wifi_mode);
    dPV("wifi ch", setting.wifi_ch);
    dPA("wifi ssid", setting.wifi_ssid, sizeof(setting.wifi_ssid));
    dPA("wifi pwd", setting.wifi_pwd, sizeof(setting.wifi_pwd));
    dPV("wifi auth", setting.wifi_auth);
    dPA("serv ip", setting.serv_addr, 4);
    dPV("serv port", setting.serv_port);

    /* Setup local data. */
    ticker.setup();

    for (int i = 0; i < leds_cnt; ++i) {
        leds[i].setup();
    }
    for (int i = 0; i < probes_cnt; ++i) {
        probes[i].setup();
    }
    for (int i = 0; i < valves_cnt; ++i) {
        valves[i].setup();
    }
    app.setup();
    wdt_reset();

    IPAddress addr(setting.serv_addr);
    p_net = setting.wifi_mode
                ? new Net(ticker, addr, setting.serv_port,
                        setting.wifi_ssid, setting.wifi_pwd)
                : new Net(ticker, addr, setting.serv_port,
                        setting.wifi_ssid, setting.wifi_pwd,
                        setting.wifi_ch, setting.wifi_auth);
    p_net->setup();
    wdt_reset();

    scheduler.setup();
    dPT(scheduler.add(task_sensors));
    dPT(scheduler.add(task_connections));
    dPT(scheduler.add(task_valves));
    dPT(scheduler.add(task_display));
    dPT(scheduler.add(task_application));
    dPT(scheduler.add(task_net));
    dPT(scheduler.add(task_reboot, REBOOT_NEXT));
    dPT(scheduler.add(task_sync));
    wdt_reset();

    iPC("setup complete");
}

/** Serial interface event. */
void serialEvent() {
    dPC("serial event");
    dPT(scheduler.force(task_net));
}

/** Main procedure. */
void loop()
{
    unsigned long delay = scheduler.run();

    if (hw_power_internal()) {
        hw_sleep();
    } else {
        hw_suspend(delay);
    }
}

static unsigned long act_time(bool sync, bool ref, bool raw)
{
    iPC("@time");

    if (sync) {
        dPC("@time: sync");
        dPT(scheduler.force(task_sync));
    }
    if (ref) {
        dPC("@time: ref");
    }
    if (raw) {
        dPC("@time: raw");
    }

    unsigned long time;

    if (raw) {
        time = ntp.get_utc_epoch(!ref);
    } else {
        unsigned long hrs = ntp.get_utc_hours(!ref);
        byte mins = ntp.get_utc_minutes(!ref);
        byte secs = ntp.get_utc_seconds(!ref);

        time = hrs << 16 | mins << 8 | secs;
    }

    return time;
}

static byte act_state(byte* buf, byte buf_max_size)
{
    iPC("@state");

    byte size = 4 + 1 * leds_cnt + 2 * probes_cnt + 3 * valves_cnt;

    if (size > buf_max_size) {
        buf[0] = size;
        return 1;
    }

    *buf++ = app.state();
    *buf++ = leds_cnt;
    for (byte i = 0; i < leds_cnt; ++i) {
        *buf++ = leds[i].mode();
    }
    *buf++ = probes_cnt;
    for (byte i = 0; i < probes_cnt; ++i) {
        *buf++ = probes[i].connection();
        *buf++ = probes[i].sensor();
    }
    *buf++ = valves_cnt;
    for (byte i = 0; i < valves_cnt; ++i) {
        *buf++ = valves[i].state_override();
        *buf++ = valves[i].state_expect();
        *buf++ = valves[i].state_actual();
    }

    return size;
}

static bool act_open(void)
{
    iPC("@open");

    bool res = true;

    for (int i = 0; i < valves_cnt; ++i) {
        res &= valves[i].force_open();
    }

    dPT(scheduler.force(task_valves));

    return res;
}

static bool act_close(void)
{
    iPC("@close");

    bool res = true;

    for (int i = 0; i < valves_cnt; ++i) {
        res &= valves[i].force_close();
    }

    dPT(scheduler.force(task_valves));

    return res;
}

static bool act_suspend(void)
{
    iPC("@suspend");

    return hw_power_save();
}

static bool act_enable(byte idx)
{
    iPV("@enable", idx);

    if (idx >= probes_cnt) {
        dPC("@enable: bad idx");
        return false;
    }

    probes[idx].enable();

    dPT(scheduler.force(task_sensors));
    dPT(scheduler.force(task_connections));

    return true;
}

static bool act_disable(byte idx, unsigned long duration)
{
    iPV("@disable", idx);

    if (idx >= probes_cnt) {
        dPC("@disable: bad idx");
        return false;
    }

    probes[idx].disable(duration);

    dPT(scheduler.force(task_sensors));
    dPT(scheduler.force(task_connections));

    return true;
}

static bool act_emu_water(byte idx, bool immediately)
{
    iPV("@emu_water", idx);

    if (idx >= probes_cnt) {
        dPC("@emu_water: bad idx");
        return false;
    }

    probes[idx].emulate_water();

    if (immediately) {
        dPC("@emu_water: immediately");
        dPT(scheduler.force(task_sensors));
    }

    return true;
}

static bool act_emu_error(byte idx, bool immediately)
{
    iPV("@emu_error", idx);

    if (idx >= probes_cnt) {
        dPC("@emu_error: bad idx");
        return false;
    }

    probes[idx].emulate_error();

    if (immediately) {
        dPC("@emu_error: immediately");
        dPT(scheduler.force(task_connections));
    }

    return true;
}

static byte act_get_wifi(byte* buf, byte buf_max_size)
{
    iPC("@get_wifi");

    const byte len = sizeof(setting.wifi_mode)
            + sizeof(setting.wifi_ch)
            + sizeof(setting.wifi_auth)
            + sizeof(setting.wifi_ssid);

    if (len > buf_max_size) {
        return 0;
    }

    *buf++ = setting.wifi_mode;
    *buf++ = setting.wifi_ch;
    *buf++ = setting.wifi_auth;
    memcpy(buf, setting.wifi_ssid, sizeof(setting.wifi_ssid));

    return len;
}

static bool act_set_wifi(const byte* buf, byte buf_size)
{
    iPC("@set_wifi");

    const byte len = sizeof(setting.wifi_mode)
            + sizeof(setting.wifi_ch)
            + sizeof(setting.wifi_auth)
            + sizeof(setting.wifi_ssid);

    if (len != buf_size) {
        return false;
    }

    setting.wifi_mode = *buf++;
    setting.wifi_ch = *buf++;
    setting.wifi_auth = *buf++;
    memcpy(setting.wifi_ssid, buf, sizeof(setting.wifi_ssid));

    return true;
}

static bool act_set_wifi_pwd(const byte* buf, byte buf_size)
{
    iPC("@set_wifi_pwd");

    const byte len = sizeof(setting.wifi_pwd);

    if (len != buf_size) {
        return false;
    }

    memcpy(setting.wifi_pwd, buf, sizeof(setting.wifi_pwd));

    return true;
}

static byte act_get_serv(byte* buf, byte buf_max_size)
{
    iPC("@get_serv");

    const byte len = sizeof(setting.serv_addr)
            + sizeof(setting.serv_port);

    if (len > buf_max_size) {
        return 0;
    }

    memcpy(buf, setting.serv_addr, sizeof(setting.serv_addr));
    buf += sizeof(setting.serv_addr);
    memcpy(buf, &setting.serv_port, sizeof(setting.serv_port));

    return len;
}

static bool act_set_serv(const byte* buf, byte buf_size)
{
    iPC("@set_serv");

    const byte len = sizeof(setting.serv_addr)
            + sizeof(setting.serv_port);

    if (len != buf_size) {
        return false;
    }

    memcpy(setting.serv_addr, buf, sizeof(setting.serv_addr));
    buf += sizeof(setting.serv_addr);
    memcpy(&setting.serv_port, buf, sizeof(setting.serv_port));

    return true;
}

static byte act_get_ntp(byte* buf, byte buf_max_size)
{
    iPC("@get_ntp");

    const byte len = sizeof(setting.ntp_host)
            + sizeof(setting.ntp_port)
            + sizeof(setting.ntp_wait);

    if (len > buf_max_size) {
        return 0;
    }

    memcpy(buf, setting.ntp_host, sizeof(setting.ntp_host));
    buf += sizeof(setting.ntp_host);
    memcpy(buf, &setting.ntp_port, sizeof(setting.ntp_port));
    buf += sizeof(setting.ntp_port);
    memcpy(buf, &setting.ntp_wait, sizeof(setting.ntp_wait));

    return len;
}

static bool act_set_ntp(const byte* buf, byte buf_size)
{
    iPC("@set_ntp");

    const byte len = sizeof(setting.ntp_host)
            + sizeof(setting.ntp_port)
            + sizeof(setting.ntp_wait);

    if (len != buf_size) {
        return false;
    }

    memcpy(setting.ntp_host, buf, sizeof(setting.ntp_host));
    buf += sizeof(setting.ntp_host);
    memcpy(&setting.ntp_port, buf, sizeof(setting.ntp_port));
    buf += sizeof(setting.ntp_port);
    memcpy(&setting.ntp_wait, buf, sizeof(setting.ntp_wait));

    return true;
}

static bool act_settings_load(bool def)
{
    iPC("@settings_load");

    dPV("@settings_load: type", def);

    return def ? (setting.defaults(), true) : setting.load();
}

static bool act_settings_save(void)
{
    iPC("@settings_save");

    setting.save();

    return true;
}

static bool act_reboot(bool def_next)
{
    iPC("@reboot");

    if (def_next) {
        dPC("@reboot: default next");
        setting.skip_next_load();
    }

    hw_reset_delay();

    return true;
}

static unsigned long task_net(unsigned long dt)
{
    dPC("#net");

    (void)dt;

    if (!p_net || p_net->is_offline()) {
        dPC("#net: offline");
        return FAR_NEXT;
    }

    ProtoSession(ticker, *p_net).action(
            act_time,
            act_state,
            act_open,
            act_close,
            act_suspend,
            act_enable,
            act_disable,
            act_emu_water,
            act_emu_error,
            act_get_wifi,
            act_set_wifi,
            act_set_wifi_pwd,
            act_get_serv,
            act_set_serv,
            act_get_ntp,
            act_set_ntp,
            act_settings_load,
            act_settings_save,
            act_reboot);

    return FAR_NEXT;
}

static unsigned long task_application(unsigned long dt)
{
    dPC("#application");

    (void)dt;

    static AppState last_state = APP_MALFUNCTION;
    AppState app_state = app.solve();

    if (last_state != app_state) {
        dPV("#application: changed", app_state);
        dPT(scheduler.force(task_display));
        dPT(scheduler.force(task_valves));
        dPT(scheduler.force(task_net));
        last_state = app_state;
    }

    return FAR_NEXT;
}

static unsigned long task_sensors(unsigned long dt)
{
    dPC("#sensors");

    (void)dt;

    static bool last_trigger = false;
    bool trigger = false;

    for (int i = 0; i < probes_cnt; ++i) {
        trigger |= PROBE_WATER == probes[i].test_sensor();
    }

    if (last_trigger != trigger) {
        dPC("#sensors: changed");
        dPT(scheduler.force(task_application));
        last_trigger = trigger;
    }

    return trigger ? PROBE_ACTIVE_NEXT : PROBE_IDLE_NEXT;
}

static unsigned long task_connections(unsigned long dt)
{
    dPC("#connections");

    (void)dt;

    static bool last_trigger = false;
    bool trigger = false;

    for (int i = 0; i < probes_cnt; ++i) {
        trigger |= PROBE_ERROR == probes[i].test_connection();
    }

    if (last_trigger != trigger) {
        dPC("#connections: changed");
        dPT(scheduler.force(task_application));
        last_trigger = trigger;
    }

    return trigger ? LINE_ACTIVE_NEXT : LINE_IDLE_NEXT;
}

static unsigned long task_display(unsigned long dt)
{
    // dPC("#disaply");

    (void)dt;

    LedMode max = LED_OFF;

    for (int i = 0; i < leds_cnt; ++i) {
        LedMode m = leds[i].mode();
        max = m > max ? m : max;
        leds[i].lit();
    }

    return max == LED_BLINK ? LED_NEXT : FAR_NEXT;
}

static unsigned long task_valves(unsigned long dt)
{
    dPC("#valves");

    (void)dt;

    static bool last_trigger = false;
    bool is_engaged = false;

    for (int i = 0; i < valves_cnt; ++i) {
        valves[i].run();
        is_engaged |= valves[i].is_overrided();
        is_engaged |= valves[i].is_engaged();
    }

    if (last_trigger != is_engaged) {
        dPC("#valves: changed");
        dPT(scheduler.force(task_net));
        dPT(scheduler.force(task_application));
        last_trigger = is_engaged;
    }

    return is_engaged ? VALVE_NEXT : FAR_NEXT;
}

static unsigned long task_reboot(unsigned long dt)
{
    dPC("#reboot");

    (void)dt;

    hw_reset();

    return REBOOT_NEXT;
}

static unsigned long task_sync(unsigned long dt)
{
    dPC("#sync");

    (void)dt;

    // ensure there is no time critical operations
    bool is_engaged = false;

    for (int i = 0; i < valves_cnt; ++i) {
        is_engaged |= valves[i].is_engaged();
    }

    if (is_engaged) {
        dPC("#sync: postpone");
        return SYNC_POSTPONE_NEXT;
    }

    // perform syncing
    if (!ntp.sync(*p_net, setting.ntp_host, setting.ntp_port, setting.ntp_wait)) {
        dPC("#sync: fails");
        return SYNC_FAIL_NEXT;
    }

    // restart tasks
    scheduler.restart();

    return SYNC_NEXT;
}

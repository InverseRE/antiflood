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
#include "app.h"
#include "hw.h"
#include "led.h"
#include "net.h"
#include "power.h"
#include "probe.h"
#include "valve.h"
#include "proto.h"
#include "scheduler.h"
#include "ticker.h"
#include "setting.h"
#include "storage.h"

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

static NetServer* p_server = nullptr;
static SettingsStorage& sst = SettingsStorage::get_instance();;
static bool storage_loaded = false;
static bool storage_created = false;

static Scheduler scheduler(ticker);

static byte act_state(byte* buf, byte buf_max_size);
static bool act_open(void);
static bool act_close(void);
static bool act_suspend(void);
static bool act_enable(byte idx);
static bool act_disable(byte idx, unsigned long duration);
static bool act_emu_water(byte idx, bool immediately);
static bool act_emu_error(byte idx, bool immediately);
static unsigned long task_server(unsigned long dt);
static unsigned long task_application(unsigned long dt);
static unsigned long task_sensors(unsigned long dt);
static unsigned long task_connections(unsigned long dt);
static unsigned long task_display(unsigned long dt);
static unsigned long task_valves(unsigned long dt);

/** Startup procedure. */
void setup()
{
    /* Debug printout. */
    DPI();
    DPC("setup");

    /* Setup HW devices */
    peripheral_configure();
    DPC("peripheral configured");

    /* Create/Load settings storage. */
    if (sst.exist()) {
        DPC("storage already exist");
        if (sst.load() != ST_OK) {
            DPC("storage loading failed");
        } else {
            DPC("storage loaded successfully");
            storage_loaded = true;
        }
    } else {
        DPC("storage not exist");
        if (sst.create() != ST_OK) {
            DPC("create failed");
        } else {
            DPC("storage created successfully");
            storage_created = true;
        }
    }

    /* Default settings. */
    byte wifi_mode = 0;
    byte ip_address[4] = {APP_DEFAULT_IP};
    unsigned short ip_port = APP_DEFAULT_PORT;
    String ssid = WIFI_DEFAULT_SSID;
    String password = WIFI_DEFAULT_PASS;
    byte channel = WIFI_DEFAULT_CHAN;
    int auth_type = WIFI_DEFAULT_SECU;

    /* Apply user settings if exist. */
    short settings_num = sst.enumerate();
    if (storage_loaded && settings_num) {
        DPC("manual configuration");
        load_setting(sst, WIFI_MODE, wifi_mode);
        DPV("WIFI_MODE", wifi_mode);
        load_setting_bytes(sst, IP_ADDRESS, ip_address);
        DPV("IP_ADDRESS", ip_address[0]);
        DPV("IP_ADDRESS", ip_address[1]);
        DPV("IP_ADDRESS", ip_address[2]);
        DPV("IP_ADDRESS", ip_address[3]);
        load_setting(sst, PORT_NUMBER, ip_port);
        DPV("PORT_NUMBER", ip_port);
        load_setting_string(sst, SSID, ssid);
        DPV("SSID", ssid);
        load_setting_string(sst, PASSWORD, password);
        DPV("PASSWORD", password);
        load_setting(sst, CHANNEL, channel);
        DPV("CHANNEL", channel);
        load_setting(sst, AUTH_TYPE, auth_type);
        DPV("AUTH_TYPE", auth_type);
    } else {
        DPC("default configuration");
    }

    IPAddress ip_addr(ip_address);

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

    if (wifi_mode) {
        p_server = new NetServer(ticker, ip_addr, ip_port, ssid, password); // STATION
    } else {
        p_server = new NetServer(ticker, ip_addr, ip_port, ssid, password, channel, auth_type); // AP
    }

    p_server->setup();

    scheduler.setup();
    DPT(scheduler.add(task_sensors));
    DPT(scheduler.add(task_connections));
    DPT(scheduler.add(task_valves));
    DPT(scheduler.add(task_display));
    DPT(scheduler.add(task_application));
    DPT(scheduler.add(task_server));

    // some delay is needed for setup take effect
    // (for example, charging detector's capacitors)
    ticker.delay_setup();

    DPC("setup complete");
}

/** Main procedure. */
void loop()
{
    unsigned long tm = ticker.tick();

    /* Application should restart once per month. */
    if (tm & 0x80000000) {
        reset();
        return;
    }

    unsigned long delay = scheduler.run();
    // TODO: check WiFi events too

    ticker.suspend(delay);
}

static byte act_state(byte* buf, byte buf_max_size)
{
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
    bool res = true;

    for (int i = 0; i < valves_cnt; ++i) {
        res &= valves[i].force_open();
    }

    DPT(scheduler.force(task_valves));

    return res;
}

static bool act_close(void)
{
    bool res = true;

    for (int i = 0; i < valves_cnt; ++i) {
        res &= valves[i].force_close();
    }

    DPT(scheduler.force(task_valves));

    return res;
}

static bool act_suspend(void)
{
    // TODO: postpone action, engage after response packet sent
    // enter_sleep(true, true);
    return false;
}

static bool act_enable(byte idx)
{
    if (idx >= probes_cnt) {
        return false;
    }

    probes[idx].enable();

    DPT(scheduler.force(task_sensors));
    DPT(scheduler.force(task_connections));

    return true;
}

static bool act_disable(byte idx, unsigned long duration)
{
    if (idx >= probes_cnt) {
        return false;
    }

    probes[idx].disable(duration);

    DPT(scheduler.force(task_sensors));
    DPT(scheduler.force(task_connections));

    return true;
}

static int act_get_setting(byte type, byte* buff, byte* len)
{
    int ret;
    DPC("act_get_setting");
    DPV("type", type);
    DPV("len", *len);

    if (!storage_loaded) {
        DPC("not loaded");
        return ST_NOT_FOUND;
    }

    if (!sst.enumerate()) {
        DPC("no records");
        return ST_NOT_FOUND;
    }

    ret = read_setting(sst, type, buff, len);
    DPV("ret", ret);

    return ret;
}

static int act_set_setting(byte type, const byte* data, byte len)
{
    int ret;
    DPC("act_set_setting");
    DPV("type", type);
    DPV("len", len);

    if (!storage_loaded && !storage_created) {
        DPC("not loaded & not created");
        return ST_NOT_FOUND;
    }

    ret = write_setting(sst, type, data, len);
    DPV("ret", ret);

    return ret;
}

static bool act_emu_water(byte idx, bool immediately)
{
    if (idx >= probes_cnt) {
        return false;
    }

    probes[idx].emulate_water();

    if (immediately) {
        DPT(scheduler.force(task_sensors));
    }

    return true;
}

static bool act_emu_error(byte idx, bool immediately)
{
    if (idx >= probes_cnt) {
        return false;
    }

    probes[idx].emulate_error();

    if (immediately) {
        DPT(scheduler.force(task_connections));
    }

    return true;
}

static unsigned long task_server(unsigned long dt)
{
    (void)dt;

    if (!p_server || p_server->is_offline() || !p_server->rx()) {
        return PROTO_NEXT;
    }

    ProtoAction action = ProtoSession(ticker, *p_server).action(
            act_state,
            act_open,
            act_close,
            act_suspend,
            act_enable,
            act_disable,
            act_get_setting,
            act_set_setting,
            act_emu_water,
            act_emu_error);

    switch (action) {
    case PROTO_STATE:       DPC("proto: state");         break;
    case PROTO_OPEN:        DPC("proto: open");          break;
    case PROTO_CLOSE:       DPC("proto: close");         break;
    case PROTO_SUSPEND:     DPC("proto: suspend");       break;
    case PROTO_EN_PROBE:    DPC("proto: enable probe");  break;
    case PROTO_DIS_PROBE:   DPC("proto: disable probe"); break;
    case PROTO_GET_SETTING: DPC("proto: get setting");   break;
    case PROTO_SET_SETTING: DPC("proto: set setting");   break;
    case PROTO_EMU_WATER:   DPC("proto: emulate water"); break;
    case PROTO_EMU_ERROR:   DPC("proto: emulate error"); break;
    case PROTO_UNKNOWN:     DPC("proto: unknown");       break;
    default:                DPC("proto: ...");
    }

    return PROTO_NEXT; // TODO: adjust timings:
                       // activate this task by actual signals from ESP01
                       // or scheduling it during intensive communications
}

static unsigned long task_application(unsigned long dt)
{
    (void)dt;

    static AppState last_state = APP_MALFUNCTION;
    AppState app_state = app.solve();

    if (last_state != app_state) {
        DPT(scheduler.force(task_display));
        DPT(scheduler.force(task_valves));
        DPT(scheduler.force(task_server));
        last_state = app_state;
    }

    return FAR_NEXT;
}

static unsigned long task_sensors(unsigned long dt)
{
    (void)dt;

    static bool last_trigger = false;
    bool trigger = false;

    for (int i = 0; i < probes_cnt; ++i) {
        trigger |= PROBE_WATER == probes[i].test_sensor();
    }

    if (last_trigger != trigger) {
        DPT(scheduler.force(task_application));
        last_trigger = trigger;
    }

    return trigger ? PROBE_ACTIVE_NEXT : PROBE_IDLE_NEXT;
}

static unsigned long task_connections(unsigned long dt)
{
    (void)dt;

    static bool last_trigger = false;
    bool trigger = false;

    for (int i = 0; i < probes_cnt; ++i) {
        trigger |= PROBE_ERROR == probes[i].test_connection();
    }

    if (last_trigger != trigger) {
        DPT(scheduler.force(task_application));
        last_trigger = trigger;
    }

    return trigger ? LINE_ACTIVE_NEXT : LINE_IDLE_NEXT;
}

static unsigned long task_display(unsigned long dt)
{
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
    (void)dt;

    static bool last_trigger = false;
    bool is_engaged = false;

    for (int i = 0; i < valves_cnt; ++i) {
        valves[i].run();
        is_engaged |= valves[i].is_overrided();
        is_engaged |= valves[i].is_engaged();
    }

    if (last_trigger != is_engaged) {
        DPT(scheduler.force(task_server));
        DPT(scheduler.force(task_application));
        last_trigger = is_engaged;
    }

    return is_engaged ? VALVE_NEXT : FAR_NEXT;
}

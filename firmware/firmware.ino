/* -*-*- mode: c++ -*-*- */

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
#include "proto.h"
#include "scheduler.h"
#include "ticker.h"

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

static Scheduler scheduler(ticker);

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

    return res;
}

static bool act_close(void)
{
    bool res = true;

    for (int i = 0; i < valves_cnt; ++i) {
        res &= valves[i].force_close();
    }

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

    return true;
}

static bool act_disable(bool idx, unsigned long duration)
{
    if (idx >= probes_cnt) {
        return false;
    }

    probes[idx].disable(duration);

    return true;
}

static unsigned long task_server(unsigned long dt)
{
    (void)dt;

    if (p_server->is_offline() || !p_server->rx()) {
        return -1;
    }

    ProtoAction action = ProtoSession(ticker, *p_server).action(
            act_state,
            act_open,
            act_close,
            act_suspend,
            act_enable,
            act_disable);

    switch (action) {
    case PROTO_STATE:     DPC("proto: state");         break;
    case PROTO_OPEN:      DPC("proto: open");          break;
    case PROTO_CLOSE:     DPC("proto: close");         break;
    case PROTO_SUSPEND:   DPC("proto: suspend");       break;
    case PROTO_EN_PROBE:  DPC("proto: enable probe");  break;
    case PROTO_DIS_PROBE: DPC("proto: disable probe"); break;
    case PROTO_UNKNOWN:   DPC("proto: unknown");       break;
    default:              DPC("proto: ...");
    }

    return -1;
}

static unsigned long task_application(unsigned long dt)
{
    (void)dt;

    AppState app_state = app.solve();

    switch (app_state) {
    case APP_OK:
        break;
    case APP_ALARM:
        DPT(scheduler.force(task_server));
        break;
    case APP_SOLVED:
        break;
    case APP_STANDBY:
        break;
    case APP_MALFUNCTION:
    default:
        DPT(scheduler.force(task_server));
    }

    return -1;
}

static unsigned long task_sensors(unsigned long dt)
{
    (void)dt;

    bool is_triggered = false;

    for (int i = 0; i < probes_cnt; ++i) {
        is_triggered |= PROBE_WATER == probes[i].test_sensor();
    }

    DPT(scheduler.force(task_application));

    return is_triggered ? PROBE_NEXT_ACTIVE : PROBE_NEXT_IDLE;
}

static unsigned long task_connections(unsigned long dt)
{
    (void)dt;

    bool is_triggered = false;

    for (int i = 0; i < probes_cnt; ++i) {
        is_triggered |= PROBE_ERROR == probes[i].test_connection();
    }

    DPT(scheduler.force(task_application));

    return is_triggered ? PROBE_NEXT_ACTIVE : PROBE_NEXT_IDLE;
}

static unsigned long task_display(unsigned long dt)
{
    (void)dt;

    LedMode min = LED_OFF;

    for (int i = 0; i < leds_cnt; ++i) {
        LedMode m = leds[i].lit();
        min = m < min ? m : min;
    }

    return    min == LED_SPIKE   ? LED_SPIKE_NEXT
            : min == LED_WARNING ? LED_FLASH_NEXT
            : min == LED_BLINK   ? LED_BLINK_NEXT
            :                      LED_NEXT;
}

static unsigned long task_valves(unsigned long dt)
{
    (void)dt;

    bool is_resolved = false;
    bool is_overrided = false;

    for (int i = 0; i < valves_cnt; ++i) {
        is_resolved |= VALVE_CLOSE == valves[i].run();
        is_overrided |= valves[i].is_overrided();
    }

    DPT(scheduler.force(task_application));

    return is_resolved && !is_overrided ? -1 : 0;
}

/** Startup procedure. */
void setup()
{
    /* Debug printout. */
    DPI();
    DPC("setup");

    /* Setup HW devices */
    peripheral_configure();
    DPC("peripheral configured");

    /* TODO: read and apply user's settings */
    IPAddress ip_addr(APP_DEFAULT_IP);
    unsigned short ip_port = APP_DEFAULT_PORT;
    String ssid = WIFI_DEFAULT_SSID;
    String password = WIFI_DEFAULT_PASS;
    byte channel = WIFI_DEFAULT_CHAN;
    int auth_type = WIFI_DEFAULT_SECU;
    DPC("custom configuration readed");

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

    // static NetServer server(ticker, ip_addr, ip_port, ssid, password); // STATION
    static NetServer server(ticker, ip_addr, ip_port, ssid, password, channel, auth_type); // AP
    server.setup();
    p_server = &server;

    scheduler.setup();
    DPT(scheduler.add(task_sensors));
    DPT(scheduler.add(task_connections));
    DPT(scheduler.add(task_display));
    DPT(scheduler.add(task_valves));
    // TODO: add event on WiFi activity

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

    ticker.delay(delay);
}

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
#include "web.h"
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

    AppState app_state = app.solve();

    if (p_server->is_offline()) {
        ticker.delay_loop();
        return;
    }

    WiFiEspClient client;
    const String& request = p_server->run(client);
    WebPage page(ticker, client);
    WebAction response = page.parse(request);

    switch (response) {

    case WEB_STATE:
        DPC("web response: state");
        page.response_state(app_state,
                leds, leds_cnt,
                probes, probes_cnt,
                valves, valves_cnt);
        break;

    case WEB_OPEN:
        DPC("web response: open");
        {
            bool is_ovr = false;
            for (int i = 0; i < valves_cnt; ++i) {
                is_ovr |= !valves[i].force_open();
            }
            page.heading(WEB_OPEN, ticker.web_heading_count(), is_ovr);
        }
        break;

    case WEB_CLOSE:
        DPC("web response: close");
        {
            bool is_ovr = false;
            for (int i = 0; i < valves_cnt; ++i) {
                is_ovr |= !valves[i].force_close();
            }
            page.heading(WEB_CLOSE, ticker.web_heading_count(), is_ovr);
        }
        break;

    case WEB_SUSPEND:
        DPC("web response: suspend");
        page.heading(WEB_SUSPEND, ticker.web_heading_count());
        ticker.delay_shield_trx();
        enter_sleep(true, true);
        break;

    case WEB_NOT_FOUND:
        DPC("web response: not found");
        page.response_not_found();
        break;

    case WEB_UNKNOWN:
    default:
        break;
    };

    ticker.delay_shield_trx();
    client.stop();

    ticker.delay_loop();
}

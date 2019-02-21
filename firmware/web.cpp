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

#include <Arduino.h>
#include "config.h" // FIXME: remove
#include "web.h"

#define ACTION_OPEN_ALL       "/open_valves"
#define ACTION_CLOSE_ALL      "/close_valves"
#define ACTION_PWR_SAVE       "/power_save_mode"

#define HTTP_RESPONSE         "HTTP/1.1 200 OK\r\n"\
                              "Content-Type: text/html\r\n"\
                              "Connection: close\r\n"

#define HTTP_NOT_FOUND        "HTTP/1.1 404 Not Found\r\n"\
                              "Content-Type: text/html\r\n"\
                              "Connection: close\r\n"

#define HTTP_BR               "\r\n"

#define HTML                  "<!DOCTYPE html>"\
                              "<html>"
#define HTML_BR               "</html>"

#define HTML_HEAD             "<head>"
#define HTML_HEAD_BR          "</head>"
#define HTML_HEAD_META        "<meta http-equiv=\"refresh\" content=\""
#define HTML_HEAD_META_BR     "\" />"
#define HTML_STYLE            "<style>"\
                              "table, th, td {"\
                              "border: 1px solid black;"\
                              "border-collapse: collapse;"\
                              "} </style>"

#define HTML_BODY             "<body>"
#define HTML_BODY_BR          "</body>"

#define HTML_ERROR            "<h2 style=\"color:brown\">bad request</h1>"

#define HTML_HEADING          "<h1 style=\"color:blue\">ANTIFLOOD SYSTEM</h1>"
#define HTML_HEADING_OPEN     "<h2 style=\"color:green\">VALVE(S) OPENING ... </h2>"
#define HTML_HEADING_CLOSE    "<h2 style=\"color:green\">VALVE(S) CLOSING ... </h2>"
#define HTML_HEADING_PWR_SAVE "<h2 style=\"color:green\">GO TO PWR SAVE MODE ... </h2>"
#define HTML_HEADING_UNKNOWN  "<h2 style=\"color:red\">ACTION NOT RECOGNIZED !!! </h2>"
#define HTML_HEADING_OK       "<h2 style=\"color:green\">OK</h2>"
#define HTML_HEADING_ALARM    "<h2 style=\"color:blue\">WATER DETECTED</h2>"
#define HTML_HEADING_SOLVED   "<h2 style=\"color:cyan\">AREA SECURED</h2>"
#define HTML_HEADING_STANDBY  "<h2 style=\"color:orange\">STANDING BY...</h2>"
#define HTML_HEADING_BAD      "<h2 style=\"color:red\">MALFUNCTION</h2>"
#define HTML_HEADING_PROBES   "<p>Probes</p>"
#define HTML_HEADING_VALVES   "<p>Valves</p>"
#define HTML_LN_BR            "<br>"

#define HTML_TABLE            "<table style=\"width:100%\">"
#define HTML_TABLE_BR         "</table>"
#define HTML_TABLE_LN         "<tr>"
#define HTML_TABLE_LN_BR      "</tr>"
#define HTML_TABLE_CAP        "<th>"
#define HTML_TABLE_CAP_BR     "</th>"
#define HTML_TABLE_ITEM       "<td>"
#define HTML_TABLE_ITEM_RED   "<td style=\"color:red\">"
#define HTML_TABLE_ITEM_GREEN "<td style=\"color:green\">"
#define HTML_TABLE_ITEM_BLUE  "<td style=\"color:blue\">"
#define HTML_TABLE_ITEM_BR    "</td>"

#define HTML_LIST             "<ul>"
#define HTML_LIST_BR          "</ul>"
#define HTML_LIST_ITEM        "<li>"
#define HTML_LIST_ITEM_RED    "<li style=\"color:red\">"
#define HTML_LIST_ITEM_GREEN  "<li style=\"color:green\">"
#define HTML_LIST_ITEM_BR     "</li>"

#define HTML_ACT_OPEN_ALL     "<form action=\"" ACTION_OPEN_ALL "\">"\
                              "<input type=\"submit\" value=\"open valves\"> </form>"
#define HTML_ACT_CLOSE_ALL    "<form action=\"" ACTION_CLOSE_ALL "\">"\
                              "<input type=\"submit\" value=\"close valves\"> </form>"
#define HTML_ACT_POWER_SAVE   "<form action=\"" ACTION_PWR_SAVE  "\">"\
                              "<input type=\"submit\" value=\"turn on power save\"> </form>"

WebPage::WebPage(const Ticker& ticker, const WiFiEspClient& client)
        : _ticker(ticker), _client(client)
{
}

void WebPage::setup(void)
{
}

WebAction WebPage::parse(const String& request)
{
    /* Main page request. */
    if (request.endsWith("/")) {
        return WEB_STATE;
    }

    /* TODO: favicon request. */
    if (request.indexOf("/favicon.ico") >= 0) {
        return WEB_NOT_FOUND;
    }

    if (request.indexOf(ACTION_OPEN_ALL) >= 0) {
        return WEB_OPEN;
    } else if (request.indexOf(ACTION_CLOSE_ALL) >= 0) {
        return WEB_CLOSE;
    } else if (request.indexOf(ACTION_PWR_SAVE) >= 0) {
        return WEB_SUSPEND;
    } else {
        return WEB_UNKNOWN;
    }
}

void WebPage::heading(WebAction action, byte count_down)
{
    _client.println(F(
                      HTTP_RESPONSE
                      HTTP_BR
                      HTML
                      HTML_HEAD
                      HTML_STYLE
                      HTML_HEAD_META));
    _client.println(count_down);
    _client.println(F(";url=http://"));
    _client.println(F(
                      APP_DEFAULT_IP_S // FIXME: replace with an actual String
                      HTML_HEAD_META_BR
                      HTML_HEAD_BR
                      HTML_BODY
                      HTML_HEADING
                      HTML_LN_BR));

    switch (action) {
    case WEB_OPEN:    _client.println(F(HTML_HEADING_OPEN));     break;
    case WEB_CLOSE:   _client.println(F(HTML_HEADING_CLOSE));    break;
    case WEB_SUSPEND: _client.println(F(HTML_HEADING_PWR_SAVE)); break;
    default:          _client.println(F(HTML_HEADING_UNKNOWN));
    }

    _client.println(F(
                      HTML_BODY_BR
                      HTML_BR
                      HTTP_BR));
}

void WebPage::response_not_found(void)
{
    _client.println(F(
                      HTTP_NOT_FOUND
                      HTTP_BR));
}

void WebPage::response_state(AppState app_state,
        const Led* leds, byte leds_cnt,
        const Probe* probes, byte probes_cnt,
        const Valve* valves, byte valves_cnt)
{
    _client.print(F(
                    /* Preambule. */
                    HTTP_RESPONSE
                    HTTP_BR
                    HTML
                    /* Head & style. */
                    HTML_HEAD
                    HTML_STYLE
                    HTML_HEAD_BR
                    /* Body. */
                    HTML_BODY
                    HTML_HEADING
                    HTML_LN_BR));
    _client.print(
              app_state == APP_OK          ? HTML_HEADING_OK
            : app_state == APP_ALARM       ? HTML_HEADING_ALARM
            : app_state == APP_SOLVED      ? HTML_HEADING_SOLVED
            : app_state == APP_STANDBY     ? HTML_HEADING_STANDBY
            : app_state == APP_MALFUNCTION ? HTML_HEADING_BAD
            :                                HTML_HEADING_BAD);

    _client.print(F(
                     /* State of probes. */
                     HTML_LN_BR
                     HTML_LN_BR
                     HTML_HEADING_PROBES
                     HTML_TABLE
                     HTML_TABLE_LN
                     HTML_TABLE_CAP "CON" HTML_TABLE_CAP_BR
                     HTML_TABLE_CAP "DET" HTML_TABLE_CAP_BR
                     HTML_TABLE_CAP "LED" HTML_TABLE_CAP_BR
                     HTML_TABLE_LN_BR));

    for (int i = 0; i < probes_cnt && i < leds_cnt; ++i) {
        ProbeConnection con = probes[i].connection();
        ProbeSensor det = probes[i].sensor();
        LedMode led = leds[i].mode();

        _client.print(F(HTML_TABLE_LN HTML_TABLE_ITEM));
        _client.print(
                  con == PROBE_OFFLINE ? F("OFFLINE")
                : con == PROBE_ONLINE  ? F("ONLINE")
                : con == PROBE_ERROR   ? F("ERROR")
                :                        F("---"));
        _client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        _client.print(
                  det == PROBE_UNKNOWN ? F("UNKNOWN")
                : det == PROBE_DRY     ? F("DRY")
                : det == PROBE_WATER   ? F("WATER")
                :                        F("---"));
        _client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        _client.print(
                  led == LED_OFF     ? F("OFF")
                : led == LED_SPIKE   ? F("SPIKE")
                : led == LED_BLINK   ? F("BLINK")
                : led == LED_ON      ? F("ON")
                : led == LED_WARNING ? F("WARNING")
                :                      F("---"));
        _client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_LN_BR));
    }

    _client.print(F(
                    HTML_TABLE_BR
                    /* State of valves. */
                    HTML_LN_BR
                    HTML_HEADING_VALVES
                    HTML_TABLE
                    HTML_TABLE_LN
                    HTML_TABLE_CAP "EXP" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "OVR" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "ACT" HTML_TABLE_CAP_BR
                    HTML_TABLE_LN_BR));

    for (int i = 0; i < valves_cnt; ++i) {
        ValveState exp = valves[i].state_expect();
        ValveState ovr = valves[i].state_override();
        ValveState act = valves[i].state_actual();

        _client.print(F(HTML_TABLE_LN HTML_TABLE_ITEM));
        _client.print(
                  exp == VALVE_IGNORE      ? F("IGNORE")
                : exp == VALVE_OPEN        ? F("OPEN")
                : exp == VALVE_CLOSE       ? F("CLOSE")
                : exp == VALVE_MALFUNCTION ? F("MALFUNCTION")
                :                            F("---"));
        _client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        _client.print(
                  ovr == VALVE_IGNORE      ? F("IGNORE")
                : ovr == VALVE_OPEN        ? F("OPEN")
                : ovr == VALVE_CLOSE       ? F("CLOSE")
                : ovr == VALVE_MALFUNCTION ? F("MALFUNCTION")
                :                            F("---"));
        _client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        _client.print(
                  act == VALVE_IGNORE      ? F("IGNORE")
                : act == VALVE_OPEN        ? F("OPEN")
                : act == VALVE_CLOSE       ? F("CLOSE")
                : act == VALVE_MALFUNCTION ? F("MALFUNCTION")
                :                            F("---"));
        _client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_LN_BR));
    }

    _client.println(F(
                      HTML_TABLE_BR
                      HTML_LN_BR
                      HTML_ACT_OPEN_ALL
                      HTML_LN_BR
                      HTML_ACT_CLOSE_ALL
                      HTML_LN_BR
                      HTML_ACT_POWER_SAVE
                      /* Conclusion. */
                      HTML_BODY_BR
                      HTML_BR
                      HTTP_BR));
}


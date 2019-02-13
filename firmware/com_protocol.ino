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



/* HTML pages. */
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

/** WEB server. */
WiFiEspServer WEB_SERVER(WEB_PORT);

/** Buffered reading for Web-requests. */
RingBuffer ibuff(WEB_IN_CACHE_SIZE);

/** WIFI shield state. true - running, false - stoped */
bool wifi_shield_state = false;

/** Configure web-server. */
static void web_configure(void)
{
    /* Start shield. */
    pinMode(WIFIEN, OUTPUT);
    digitalWrite(WIFIEN, HIGH);
    pinMode(WIFIRS, OUTPUT);
    digitalWrite(WIFIRS, HIGH);

    delay(SHIELD_STARTUP_TIME);

    Serial.begin(SHIELD_BAUD_RATE);
    WiFi.init(&Serial);
    if (WiFi.status() == WL_NO_SHIELD) {
        DP("esp8266 shield is not responsive");
        power_save_mode_on();
        return;
    }
    DP("esp8266 online");

    /* Configure network. */
    IPAddress ip_addr(WEB_IP);

#if defined WIFI_STATION
    WiFi.config(ip_addr);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
#elif defined WIFI_ACCESS_POINT
    WiFi.configAP(ip_addr);
    WiFi.beginAP(WIFI_SSID, WIFI_CH, WIFI_PASS, WIFI_SECU);
#else
#   error WiFi unknown mode
#endif
    DP("wifi configured");

    /* Start server. */
    WEB_SERVER.begin();
    /* Set WIFI flag. */
    wifi_shield_state = true;
    DP("web-server started");
}

/** Checks client request. */
static void http_parse_request(WiFiEspClient client, const String& rbuff)
{
    /* Main page request. */
    if (rbuff.endsWith("/")) {
        http_response(client);
        return;
    }

    /* TODO: favicon request. */
    if (rbuff.indexOf("/favicon.ico") >= 0) {
        http_response_not_found(client);
        return;
    }

    /* Check for valves action. Action blocks http server for a long time. Send answer to user before perform action. */
    if (rbuff.indexOf(ACTION_OPEN_ALL) >= 0 || rbuff.indexOf(ACTION_OPEN_ALL "?") >= 0) {
        http_action_response(client, VALVE_OPENING_ACTION, 1);
        valves_force_open();
    } else if (rbuff.indexOf(ACTION_CLOSE_ALL) >= 0 || rbuff.indexOf(ACTION_CLOSE_ALL "?") >= 0) {
        http_action_response(client, VALVE_CLOSING_ACTION, 1);
        valves_force_close();
    } else if (rbuff.indexOf(ACTION_PWR_SAVE) >= 0 || rbuff.indexOf(ACTION_PWR_SAVE "?") >= 0) {
        http_action_response(client, SHIELD_PWR_SAVE_ACTION, 1);
        delay(100); /* Some delay to finish trunsmit. */
        power_save_mode_on();
    } else {
        /* TODO: will be removed after debug. Let me know if GET request not parsed. */
        http_action_response(client, -1, 1);
    }
}

/** Response for valves open/close action with redirection to the main page. */
static void http_action_response(WiFiEspClient client, unsigned char act, unsigned char timeout)
{
    client.println(F(
                    HTTP_RESPONSE
                    HTTP_BR
                    HTML
                    HTML_HEAD
                    HTML_STYLE
                    HTML_HEAD_META));
    client.println(timeout);
    client.println(F(";url=http://"));
    client.println(F(
                    WEB_IP_STR
                    HTML_HEAD_META_BR
                    HTML_HEAD_BR
                    HTML_BODY
                    HTML_HEADING
                    HTML_LN_BR));
    if (VALVE_OPENING_ACTION == act) {
        client.println(F(HTML_HEADING_OPEN));
    } else if (VALVE_CLOSING_ACTION == act) {
        client.println(F(HTML_HEADING_CLOSE));
    } else if (SHIELD_PWR_SAVE_ACTION == act) {
        client.println(F(HTML_HEADING_PWR_SAVE));
    } else {
        client.println(F(HTML_HEADING_UNKNOWN));
    }
    client.println(F(
                    HTML_BODY_BR
                    HTML_BR
                    HTTP_BR));
}


/** Construct an HTML-page of an actual status. */
static void http_response(WiFiEspClient client)
{
    int i = PROBES_CNT;
    int j = VALVES_CNT;

    client.print(F(
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
    client.print(
              OVERALL_STATE == APP_OK          ? HTML_HEADING_OK
            : OVERALL_STATE == APP_ALARM       ? HTML_HEADING_ALARM
            : OVERALL_STATE == APP_SOLVED      ? HTML_HEADING_SOLVED
            : OVERALL_STATE == APP_MALFUNCTION ? HTML_HEADING_BAD
            :                                    HTML_HEADING_BAD);

    client.print(F(
                    /* State of probes. */
                    HTML_LN_BR
                    HTML_LN_BR
                    HTML_HEADING_PROBES
                    HTML_TABLE
                    HTML_TABLE_LN
                    HTML_TABLE_CAP "PORT"  HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "VALUE" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "dU"    HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "dt"    HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "CON"   HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "DET"   HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "LED"   HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "MODE"  HTML_TABLE_CAP_BR
                    HTML_TABLE_LN_BR));

    while (i--) {
        probe_live_state_t cst = PROBES[i].con;
        probe_detector_state_t dst = PROBES[i].det;
        led_state_t lm = PROBES[i].mode;

        client.print(F(HTML_TABLE_LN HTML_TABLE_ITEM));
        client.print(String(PROBES[i].port));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(PROBES[i].val));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(PROBES[i].chk));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(PROBES[i].elt));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(
                  cst == PROBE_OFFLINE ? F("OFFLINE")
                : cst == PROBE_ONLINE  ? F("ONLINE")
                : cst == PROBE_ERROR   ? F("ERROR")
                :                        F("---"));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(
                  dst == PROBE_DRY   ? F("DRY")
                : dst == PROBE_WATER ? F("WATER")
                :                      F("---"));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(PROBES[i].led));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(
                  lm == LED_OFF     ? F("OFF")
                : lm == LED_SPIKE   ? F("SPIKE")
                : lm == LED_BLINK   ? F("BLINK")
                : lm == LED_ON      ? F("ON")
                : lm == LED_WARNING ? F("WARNING")
                :                     F("---"));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_LN_BR));
    }

    client.print(F(
                    HTML_TABLE_BR
                    /* State of valves. */
                    HTML_LN_BR
                    HTML_HEADING_VALVES
                    HTML_TABLE
                    HTML_TABLE_LN
                    HTML_TABLE_CAP "EXP ST" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "ACT ST" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "T"      HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "U(I)"   HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "A PORT" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "I PORT" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "O PORT" HTML_TABLE_CAP_BR
                    HTML_TABLE_CAP "C PORT" HTML_TABLE_CAP_BR
                    HTML_TABLE_LN_BR));

    while (j--) {
        valve_state_t ves = VALVES[j].exp_state;
        valve_state_t vas = VALVES[j].act_state;

        client.print(F(HTML_TABLE_LN HTML_TABLE_ITEM));
        client.print(
                  ves == VALVE_IGNORE      ? F("IGNORE")
                : ves == VALVE_OPEN        ? F("OPEN")
                : ves == VALVE_CLOSE       ? F("CLOSE")
                : ves == VALVE_MALFUNCTION ? F("MALFUNCTION")
                :                            F("---"));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(
                  vas == VALVE_IGNORE      ? F("IGNORE")
                : vas == VALVE_OPEN        ? F("OPEN")
                : vas == VALVE_CLOSE       ? F("CLOSE")
                : vas == VALVE_MALFUNCTION ? F("MALFUNCTION")
                :                            F("---"));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(VALVES[j].elt));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(VALVES[j].chk));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(VALVES[j].fport));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(VALVES[j].vport));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(VALVES[j].oport));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_ITEM));
        client.print(String(VALVES[j].cport));
        client.print(F(HTML_TABLE_ITEM_BR HTML_TABLE_LN_BR));
    }

    client.println(F(
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


/** Simple response for favicon request from browser. */
static void http_response_not_found(WiFiEspClient client)
{
    client.println(F(
                    HTTP_NOT_FOUND
                    HTTP_BR));
}

/** Process web-server events. */
static void web_run(void)
{
    WiFiEspClient client = WEB_SERVER.available();

    if (client) {
        String rcv_url = "";
        bool stage_1 = true;
        bool stage_2 = false;

        DP("client appears");

        ibuff.init();

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                ibuff.push(c);

                /* Search for GET request. */
                if (stage_1 && ibuff.endsWith("GET ")) {
                    stage_1 = false;
                    stage_2 = true;
                    continue;
                }

                /* Store requested URL. */
                if (stage_2) {
                    rcv_url += c;
                    if (c == ' ') {
                        stage_2 = false;
                        rcv_url.trim();
                    }
                }

                /* Two newline characters in a row is the end of the HTTP request. */
                if (ibuff.endsWith("\r\n\r\n")) {
                    DP("data received");
                    if (!stage_1 && !stage_2) {
                        http_parse_request(client, rcv_url);
                    } else {
                        /* Let me know if GET request not parsed. Back to the main page in 1 second. */
                        http_action_response(client, -2, 1);
                    }
                    DP("data sent");
                    break;
                }
            }
        }
        if (!client.connected()) {
            DP("client disconnected");
            delay(1);
        }

        /* Time to complete transmission. */
        delay(WEB_TRX_LATENCY);

        /* Disconnect. */
        client.stop();
        DP("client dropped");
    }
}

/** Return actual wifi shield state. */
static bool get_wifi_state()
{
    return wifi_shield_state;
}

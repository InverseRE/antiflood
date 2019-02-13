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



/* TODO: let the probes be disabled by hand. */

/** Probe sensor states. */
typedef enum {
    PROBE_DRY,                              /**< nothing happens, all is OK */
    PROBE_WATER                             /**< some activity detected */
} probe_detector_state_t;

/** Probe connection status. */
typedef enum {
    PROBE_OFFLINE,                          /**< disconnected or non-controlled probe */
    PROBE_ONLINE,                           /**< probe is connected */
    PROBE_ERROR                             /**< shortcircuit, malfunction or bad readings */
} probe_live_state_t;

/** Probe. */
typedef struct {
    byte port;                              /**< analog input port */
    byte val;                               /**< readed static value */
    byte chk;                               /**< readed dynamic value (connection check) */
    byte elt;                               /**< elapsed time (connection check), normalized */
    probe_detector_state_t det;             /**< probe detector state */
    probe_live_state_t con;                 /**< probe connection state */
    byte led;                               /**< linked LED */
    led_state_t mode;                       /**< linked LED's status */
} probe_t;



/** List of available probes. */
static probe_t PROBES[] = {
    {PROBE0, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED0, LED_OFF},
    {PROBE1, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED1, LED_OFF},
    {PROBE2, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED2, LED_OFF},
    {PROBE3, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED3, LED_OFF},
    {PROBE4, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED4, LED_OFF},
    {PROBE5, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED5, LED_OFF}
};

/** Count of probes. */
int PROBES_CNT = (sizeof(PROBES) / sizeof(PROBES[0]));



/** Configure and fast check for probes. */
static void probes_configure(void)
{
    int i = PROBES_CNT;

    while (i--) {
        pinMode(PROBES[i].port, INPUT_PULLUP);
    }

    DP("input lines configured");
}

/** Perform readings from probes. */
static void probes_test(void)
{
    int i = PROBES_CNT;

    /*
     * Perhaps some delay needed,
     * in case this procedure is beign called right after probes_check().
     */

    /* Normalize to a byte-range value. */
    while (i--) {
        PROBES[i].val = analogRead(PROBES[i].port) >> 2;
    }
}

/** Check connection status for probes. */
static void probes_check(void)
{
    int i = PROBES_CNT;
    unsigned long elt, tm1, tm2;

    /* Start discharging capacitors. */
    while (i--) {
        pinMode(PROBES[i].port, OUTPUT);
        digitalWrite(PROBES[i].port, LOW);
    }

    /* Fully discharge capasitors. */
    delay(PROBE_DISCH_DURATION);

    /* Prepare to read values. */
    tm1 = millis();

    i = PROBES_CNT;
    while (i--) {
        pinMode(PROBES[i].port, INPUT_PULLUP);
    }

    /* Perform some delay to partially charge back. */
    delay(PROBE_CHECK_DURATION);

    /* Get voltage at the moment. */
    tm2 = millis();
    elt = (tm2 - tm1) / 4;
    elt = elt == 0 ? 1 : elt;
    elt = elt < 255 ? elt : 255;

    i = PROBES_CNT;
    while (i--) {
        PROBES[i].chk = analogRead(PROBES[i].port) >> 2;
        PROBES[i].elt = (byte)elt;
    }
}

/** Calculate probes' states. */
static void probes_result(void)
{
    int i = PROBES_CNT;

    OVERALL_STATE = APP_OK;

    while (i--) {
        probe_detector_state_t dst;
        probe_live_state_t cst;
        byte v = PROBES[i].val;
        byte c = PROBES[i].chk;
        byte t = PROBES[i].elt;
        app_state_t app_st;

        dst = PROBES[i].det = v < PROBE_V_FLOOD_TRIGGER ? PROBE_WATER : PROBE_DRY;
        cst = PROBES[i].con =
                  v < PROBE_V_SHORT_CIRCUIT ? PROBE_ERROR
                : t == 0                    ? PROBE_OFFLINE
                : c > v * CG_MAX_FACTOR     ? PROBE_OFFLINE
                : c > CG_MIN                ? PROBE_ONLINE
                :                             PROBE_ERROR;

        PROBES[i].mode =
                  cst == PROBE_ERROR   ? LED_WARNING
                : dst == PROBE_WATER   ? LED_BLINK
                : cst == PROBE_ONLINE  ? LED_SPIKE
                : dst == PROBE_DRY     ? LED_OFF
                : cst == PROBE_OFFLINE ? LED_OFF
                :                        LED_ON;

        app_st =  cst == PROBE_ERROR   ? APP_MALFUNCTION
                : dst == PROBE_WATER   ? APP_ALARM
                : cst == PROBE_ONLINE  ? APP_OK
                : dst == PROBE_DRY     ? APP_OK
                : cst == PROBE_OFFLINE ? APP_OK
                :                        APP_MALFUNCTION;

        OVERALL_STATE = app_st > OVERALL_STATE ? app_st : OVERALL_STATE;
    }
}

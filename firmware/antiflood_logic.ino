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

#define PROBE0    A0                        /**< probe 0 */
#define PROBE1    A1                        /**< probe 1 */
#define PROBE2    A2                        /**< probe 2 */
#define PROBE3    A3                        /**< probe 3 */
#define PROBE4    A4                        /**< probe 4 */
#define PROBE5    A5                        /**< probe 5 */
#define VFOPST    A6                        /**< valve's forced state */
#define VCONSC    A7                        /**< valve supply current */

#define LED0      8                         /**< LED 0 */
#define LED1      9                         /**< LED 1 */
#define LED2      10                        /**< LED 2 */
#define LED3      11                        /**< LED 3 */
#define LED4      12                        /**< LED 4 */
#define LED5      13                        /**< LED 5 */

#define OPEN      7                         /**< open valves */
#define CLOSE     6                         /**< close valves */
#define TRIG_LVL  HIGH                      /**< action engage level */
#define IDLE_LVL  LOW                       /**< idle level */

/* Probes settings. */
#define PROBE_V_SHORT_CIRCUIT   5           /**< lower value is treated as short circuit */
#define PROBE_V_FLOOD_TRIGGER   175         /**< lower value is treated like a signal */
#define PROBE_TEST_PERIOD       5000        /**< signal reading period, ms */
#define PROBE_CHECK_PERIOD      10000       /**< connection verifying period, ms */
#define PROBE_DISCH_DURATION    1           /**< discharge capcitor delay, ms */
#define PROBE_CHECK_DURATION    1           /**< measurement waitng delay: t = sqrt(R*C), ms */
#define CG_MIN                  10          /**< minimal voltage on capasitor, normalized */
#define CG_MAX_FACTOR           0.8         /**< factor of maximal voltage on capasitor */

/* Valves settings. */
#define VALVE_DURATION_AUTO     6000        /**< given time to perform an action before retry, alarm or halt */
#define VALVE_DURATION_FORCED   20000       /**< given time to perform a forced action before alarm or halt */
#define VALVE_CURRENT_MIN       5           /**< normalized value of minimal current of valves */
#define VALVE_CURRENT_MAX       25          /**< normalized value of maximum current of valves */
#define VALVE_OPENING_TIME      5000        /**< amount of time to open valve, ms*/
#define VALVE_CLOSING_TIME      6000        /**< amount of time to close valve, ms*/
#define VALVE_FORCEOPENING_TIME 10000       /**< amount of time to force valve opening, ms*/
#define VALVE_FORCECLOSING_TIME 15000       /**< amount of time to force valve closing, ms*/
#define VALVE_OPENING_ACTION    0           /**< valve opening action identifier */
#define VALVE_CLOSING_ACTION    1           /**< valve closing action identifier */


/** LED states. */
typedef enum {
    LED_OFF,                                /**< switched off */
    LED_SPIKE,                              /**< tiny light or rapid single flash */
    LED_BLINK,                              /**< periodic blinking */
    LED_ON,                                 /**< constantly on */
    LED_WARNING                             /**< rapidly flashes */
} led_state_t;

/** Probe states. */
typedef enum {
    PROBE_DRY,                              /**< nothing happens, all is OK */
    PROBE_WATER                             /**< some activity detected */
} probe_detector_state_t;

/** Probe states. */
typedef enum {
    PROBE_OFFLINE,                          /**< disconnected or non-controlled probe */
    PROBE_ONLINE,                           /**< probe is connected */
    PROBE_ERROR                             /**< shortcircuit, malfunction or bad readings */
} probe_live_state_t;

/** Valve states and controls. */
typedef enum {
    VALVE_IGNORE,                           /**< valve disconnected or ignored */
    VALVE_OPEN,                             /**< 'open' state */
    VALVE_CLOSE,                            /**< 'close' state */
    VALVE_MALFUNCTION,                      /**< some malfunction detected */
} valve_state_t;

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

/** Valve. */
typedef struct {
    valve_state_t exp_state;                /**< valve state to get */
    valve_state_t act_state;                /**< actual valve state */
    unsigned long elt;                      /**< duration of a current action, ms */
    byte chk;                               /**< self check readings */
    byte fport;                             /**< input port for forced operation */
    byte vport;                             /**< input self check port */
    byte oport;                             /**< port to perform an 'open' action */
    byte cport;                             /**< port to perform a 'close' action */
} valve_t;

/** List of available probes. */
static probe_t PROBES[] = {
    {PROBE0, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED0, LED_OFF},
    {PROBE1, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED1, LED_OFF},
    {PROBE2, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED2, LED_OFF},
    {PROBE3, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED3, LED_OFF},
    {PROBE4, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED4, LED_OFF},
    {PROBE5, 0, 0, 0, PROBE_DRY, PROBE_OFFLINE, LED5, LED_OFF}
};

/* TODO: let the probes be disabled by hand. */

/** Application overall state. */
app_state_t OVERALL_STATE = APP_MALFUNCTION;

/** Count of probes. */
int PROBES_CNT = (sizeof(PROBES) / sizeof(PROBES[0]));

/** List of presented valves. */
static valve_t VALVES[] = {
    {VALVE_IGNORE, VALVE_IGNORE, 0, 0, VFOPST, VCONSC, OPEN, CLOSE}
};

/** Count of valves. */
int VALVES_CNT = (sizeof(VALVES) / sizeof(VALVES[0]));

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

/** Configure and fast check for valves. */
static void valves_configure(void)
{
    int j = VALVES_CNT;

    while (j--) {
        pinMode(VALVES[j].fport, INPUT);
        pinMode(VALVES[j].vport, INPUT);
        pinMode(VALVES[j].oport, OUTPUT);
        pinMode(VALVES[j].cport, OUTPUT);

        digitalWrite(VALVES[j].oport, IDLE_LVL);
        digitalWrite(VALVES[j].cport, IDLE_LVL);
    }

    DP("valves control lines configured");
}


/** Choose the next move. */
static void valves_solve(void)
{
    valve_state_t gst = VALVE_IGNORE;

    int i = PROBES_CNT;
    int j = VALVES_CNT;

    while (i--) {
        const probe_live_state_t cst = PROBES[i].con;
        const probe_detector_state_t dst = PROBES[i].det;
        const valve_state_t vst =
                  cst == PROBE_ERROR   ? VALVE_CLOSE
                : dst == PROBE_WATER   ? VALVE_CLOSE
                : cst == PROBE_ONLINE  ? VALVE_IGNORE
                : dst == PROBE_DRY     ? VALVE_IGNORE
                : cst == PROBE_OFFLINE ? VALVE_IGNORE
                :                        VALVE_CLOSE;

        gst = gst == VALVE_IGNORE ? vst : gst;
    }

    while (j--) {
        VALVES[j].exp_state = gst == VALVE_IGNORE ? VALVES[j].exp_state : gst;
    }
}


/** Perform an action. */
static void valves_run(void)
{
    int j = VALVES_CNT;

    while (j--) {
        const valve_state_t act = VALVES[j].act_state;
        const valve_state_t exp = VALVES[j].exp_state;
        const byte open = VALVES[j].oport;
        const byte close = VALVES[j].cport;

        switch (exp) {

        case VALVE_IGNORE:
            /* Do nothing. */
            break;

        case VALVE_OPEN:
            /* If actual state should be ignored, or malfunction happened,
               or already opened then do stop
               but at first power down the other line. */
            digitalWrite(close, IDLE_LVL);
            digitalWrite(open, act == VALVE_CLOSE ? TRIG_LVL : IDLE_LVL);
            break;

        case VALVE_CLOSE:
            /* If actual state should be ignored, or malfunction happened,
               or already closed then do stop
               but at first power down the other line. */
            digitalWrite(open, IDLE_LVL);
            digitalWrite(close, act == VALVE_OPEN ? TRIG_LVL : IDLE_LVL);
            break;

        case VALVE_MALFUNCTION:
        default:
            OVERALL_STATE = APP_MALFUNCTION;
            DP("valves fail");

            /* Stop operations. */
            digitalWrite(open, IDLE_LVL);
            digitalWrite(close, IDLE_LVL);
        }
    }
}


/** Check actual status. */
static void valves_check(void)
{
    int j = VALVES_CNT;

    while (j--) {
        const valve_state_t exp = VALVES[j].exp_state;
        const valve_state_t act = VALVES[j].act_state;
        const unsigned long elt = VALVES[j].elt;
        const unsigned long limit = exp == VALVE_OPEN ? VALVE_OPENING_TIME : VALVE_CLOSING_TIME;
        unsigned long tm = millis();

        /* TODO: supply current readings are unused for now */
        /* byte val = analogRead(VALVES[j].vport) >> 2; */
        /* bool is_idle = val < VALVE_CURRENT_MIN; */
        /* bool is_high = val > VALVE_CURRENT_MAX; */

        /* VALVES[j].act_state = */
        /*           is_high ? VALVE_MALFUNCTION */
        /*         : is_idle ? exp */
        /*         :           VALVE_IGNORE; */

        /* If action completed. */
        if (exp == act) {
            app_state_t app_st = exp == VALVE_CLOSE ? APP_SOLVED : OVERALL_STATE;
            OVERALL_STATE = app_st > OVERALL_STATE ? app_st : OVERALL_STATE;
        }

        /* Skip completed or bad states. */
        if (exp == act || exp == VALVE_IGNORE || exp == VALVE_MALFUNCTION) {
            continue;
        }

        /* Start or limit the operation time. */
        if (elt == 0) {
            VALVES[j].act_state = exp == VALVE_OPEN ? VALVE_CLOSE : VALVE_OPEN;
            VALVES[j].elt = tm;
        } else if (tm - elt > limit) {
            VALVES[j].act_state = exp;
            VALVES[j].elt = 0;
        }
    }
}


/**
 * Check if any not complete valves actions.
 *
 * @return 1 - there is some not complete actions
 * @return 0 - there is no uncomplete actions
 */
static int is_valves_actions(void)
{
    int j = VALVES_CNT;

    while (j--) {
        const valve_state_t exp = VALVES[j].exp_state;
        const valve_state_t act = VALVES[j].act_state;

        /* Skip completed or bad states. */
        if (exp == act || exp == VALVE_IGNORE || exp == VALVE_MALFUNCTION) {
            continue;
        }

        /* Valve not complete action. */
        if (exp != act) {
            return 1;
        }
    }

    return 0;
}

/** Force valves to open. */
static void valves_force_open()
{
    int j = VALVES_CNT;

    while (j--) {
        VALVES[j].act_state = VALVE_IGNORE;
        VALVES[j].exp_state = VALVE_OPEN;
        VALVES[j].elt = 0;

        digitalWrite(VALVES[j].cport, IDLE_LVL);
        digitalWrite(VALVES[j].oport, TRIG_LVL);
    }

    delay(VALVE_FORCEOPENING_TIME);

    j = VALVES_CNT;
    while (j--) {
        digitalWrite(VALVES[j].oport, IDLE_LVL);

        VALVES[j].act_state = VALVE_OPEN;
        VALVES[j].exp_state = VALVE_OPEN;
    }

    OVERALL_STATE = APP_MALFUNCTION != OVERALL_STATE ? APP_OK : OVERALL_STATE;
}

/** Force valves to close. */
static void valves_force_close()
{
    int j = VALVES_CNT;

    while (j--) {
        VALVES[j].act_state = VALVE_IGNORE;
        VALVES[j].exp_state = VALVE_CLOSE;
        VALVES[j].elt = 0;

        digitalWrite(VALVES[j].oport, IDLE_LVL);
        digitalWrite(VALVES[j].cport, TRIG_LVL);
    }

    delay(VALVE_FORCECLOSING_TIME);

    j = VALVES_CNT;
    while (j--) {
        digitalWrite(VALVES[j].cport, IDLE_LVL);

        VALVES[j].act_state = VALVE_CLOSE;
        VALVES[j].exp_state = VALVE_CLOSE;
    }

    OVERALL_STATE = APP_MALFUNCTION != OVERALL_STATE ? APP_OK : OVERALL_STATE;
}

/** Check if app state queals to given state. */
static bool check_app_state(app_state_t state)
{
    return OVERALL_STATE == state;
}

/** Check if app state queals to given state. */
static void set_app_state(app_state_t state)
{
    OVERALL_STATE == state;
}

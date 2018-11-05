/* -*- mode: c -*- */

/*
 * Antiflood Copyright (C) 2018 Alexey <Inverse> Shumeiko
 *
 * This file is part of Antiflood project.
 *
 * This firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the firmware. If not, see <http://www.gnu.org/licenses/>.
 *
 * For more details see LICENSE file.
 */



#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>



/* Uncomment the following define to use software serial i/o for debug printing purpose. */
// #define DEBUG_PRINTOUT



#define PROBE0    A0                        /**< probe 0 */
#define PROBE1    A1                        /**< probe 1 */
#define PROBE2    A2                        /**< probe 2 */
#define PROBE3    A3                        /**< probe 3 */
#define PROBE4    A4                        /**< probe 4 */
#define PROBE5    A5                        /**< probe 5 */
#define VFOPST    A6                        /**< valve's forced state */
#define VCONSC    A7                        /**< valve supply current */

#define WIFIRX    0                         /**< WiFi-shield RX */
#define WIFITX    1                         /**< WiFi-shield TX */
#define WIFIEN    2                         /**< enable WiFi-shield (active high) */
#define WIFIRS    3                         /**< reset WiFi-shield (active low) */

#define OPEN      7                         /**< open valves */
#define CLOSE     6                         /**< close valves */
#define TRIG_LVL  HIGH                      /**< action engage level */
#define IDLE_LVL  LOW                       /**< idle level */

#define LED0      8                         /**< LED 0 */
#define LED1      9                         /**< LED 1 */
#define LED2      10                        /**< LED 2 */
#define LED3      11                        /**< LED 3 */
#define LED4      12                        /**< LED 4 */
#define LED5      13                        /**< LED 5 */

#define SW_RX     4                         /**< software serial rx (debug printout) */
#define SW_TX     5                         /**< software serial tx (debug printout) */



#define USE_POWER_SAVE                      /**< use power saving mode between loop cycles */



#define WIFI_STATION                        /**< connect to an existed network */
//#define WIFI_ACCESS_POINT                   /**< start own network */
#define WIFI_SSID ">)"                      /**< network identifier */
#define WIFI_PASS "00000000"                /**< network access password */
#define WIFI_CH   10                        /**< network channel number */
#define WIFI_SECU ENC_TYPE_WPA2_PSK         /**< network security type */

#define WEB_IP     192, 168, 111, 111       /**< device's IP address */
#define WEB_IP_STR "192.168.111.111"        /**< device's IP address string */
#define WEB_PORT   80                       /**< server port */



#define PROBE_V_SHORT_CIRCUIT   5           /**< lower value is treated as short circuit */
#define PROBE_V_FLOOD_TRIGGER   175         /**< lower value is treated like a signal */
#define PROBE_TEST_PERIOD       5000        /**< signal reading period, ms */
#define PROBE_CHECK_PERIOD      10000       /**< connection verifying period, ms */
#define PROBE_DISCH_DURATION    1           /**< discharge capcitor delay, ms */
#define PROBE_CHECK_DURATION    1           /**< measurement waitng delay: t = sqrt(R*C), ms */
#define CG_MIN                  10          /**< minimal voltage on capasitor, normalized */
#define CG_MAX_FACTOR           0.8         /**< factor of maximal voltage on capasitor */

#define LED_SPIKE_DURATION      30          /**< spike on duration, ms */
#define LED_FLASH_DURATION      200         /**< rapid flashing on/off duration, ms */
#define LED_BLINK_DURATION      1000        /**< blink on/off duration, ms */

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

#define SHIELD_STARTUP_TIME     1000        /**< startup time for ESP8266, ms */
#define SHIELD_BAUD_RATE        115200      /**< shield's UART baud rate */

#define WEB_TRX_LATENCY         10          /**< some delays in web communication, ms */
#define WEB_IN_CACHE_SIZE       32          /**< size of input buffer for http */

#define DEBUG_BAUD_RATE         115200      /**< software serial baud rate */



/** Overall detector states. */
typedef enum {
    APP_OK          = 0,                    /**< all is good */
    APP_ALARM       = 1,                    /**< water detected */
    APP_SOLVED      = 2,                    /**< valves closed */
    APP_MALFUNCTION = 3                     /**< something is wrong */
} app_state_t;

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

/* TODO: let the probes be disabled by hand */

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
#define PROBES_CNT (sizeof(PROBES) / sizeof(PROBES[0]))

/** List of presented valves. */
static valve_t VALVES[] = {
    {VALVE_IGNORE, VALVE_IGNORE, 0, 0, VFOPST, VCONSC, OPEN, CLOSE}
};

/** Count of valves. */
#define VALVES_CNT (sizeof(VALVES) / sizeof(VALVES[0]))

/** WEB server. */
WiFiEspServer WEB_SERVER(WEB_PORT);

/** Application overall state. */
app_state_t OVERALL_STATE = APP_MALFUNCTION;

/** Buffered reading for Web-requests. */
RingBuffer ibuff(WEB_IN_CACHE_SIZE);



/** Debug printout. */
#ifdef DEBUG_PRINTOUT
SoftwareSerial SWS(SW_RX, SW_TX);
#define DP(str) SWS.println(F(str));
#define DPS(str) SWS.println(str);
#else
#define DP(str)
#define DPS(str)
#endif


/* HTML pages. */
#define ACTION_OPEN_ALL       "/open_valves"
#define ACTION_CLOSE_ALL      "/close_valves"

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



/** Is action on valve forced? */
#define IS_FORCED(vs) (VALVE_FORCE_OPENING == (vs) || VALVE_FORCE_CLOSING == (vs))

/** Watchdog Interrupt Service. This is executed when watchdog timed out. */
ISR(WDT_vect) {
  /* May be added counter to increase sleep time. */
}

/** Turn off unused modules at startup. */
static void peripheral_configure()
{
    /* Disable not used timers. */
    power_timer1_disable();
    power_timer2_disable();

    /* Disable I2C. */
    power_twi_disable();

    /* Disable SPI. */
    power_spi_disable();
}

/** Enters the arduino into a sleep mode. */
static void enter_sleep(boolean adc_off, boolean bod_off)
{
    int previousADCSRA;

    /* Setup watchdog. */
    wdt_setup();

    /* disable ADC */
    if (adc_off) {
        previousADCSRA = ADCSRA;
        ADCSRA = 0;
    }

    /* Use the power saving mode. */
    /* Only SLEEP_MODE_IDLE allow not turn off UART. */
    /* TODO: use POWER_DOWN */
    set_sleep_mode(SLEEP_MODE_IDLE);

    /* Disable interrupts. */
    noInterrupts();
    sleep_enable();

    /* Disable peripherals. */
    if (adc_off) {
        power_adc_disable();
    }

    /* Turn off brown-out enable in software. */
    if (bod_off) {
        MCUCR = bit(BODS) | bit(BODSE);
        MCUCR = bit(BODS);
    }

    /* Enable interrupts. */
    interrupts();

    /* Now enter sleep mode. */
    sleep_mode();

    /* The program will continue from here after the WDT timeout */

    /* First thing to do is disable sleep. */
    sleep_disable();

    /* Re-enable peripherals. */
    if (adc_off) {
        power_adc_enable();
    }

    /* Delay to settle down ADC and peripheries. */
    delay(10);

    /* Set previous ADC config. */
    ADCSRA = previousADCSRA;

    /* Turn off WDT. */
    wdt_disable();
}

/**
 * Setup the Watch Dog Timer (WDT).
 * WDT will generate interrupt without reset in about 1sec.
 */
static void wdt_setup() {

    /* Clear the reset flag on the MCUSR, the WDRF bit (bit 3). */
    MCUSR &= ~(1 << WDRF);

    /* Configure the Watchdog timer Control Register (WDTCSR). */
    WDTCSR |= (1 << WDCE) | (1 << WDE);

    /* Setting the watchdog pre-scaler value. */
    WDTCSR  = (0 << WDP3) | (1 << WDP2) | (1 << WDP1) | (0 << WDP0);

    /* Enable the WD interrupt (note: no reset). */
    WDTCSR |= _BV(WDIE);
}


/** Signal an error by all means. */
static void halt_on_error(void)
{
    int i = PROBES_CNT;

    DP("halt");

    WiFi.disconnect();
    delay(10);
    pinMode(WIFIEN, OUTPUT);
    digitalWrite(WIFIEN, LOW);
    pinMode(WIFIRS, OUTPUT);
    digitalWrite(WIFIRS, LOW);

    while (i--) {
        pinMode(PROBES[i].led, OUTPUT);
    }

    do {
        unsigned long tm = millis();
        byte sig = tm % (  2 * LED_FLASH_DURATION) > LED_FLASH_DURATION ? LOW : HIGH;

        i = PROBES_CNT;
        while (i--) {
            digitalWrite(PROBES[i].led, sig);
        }

        delay(10);

    } while (true); /* here may be placed an option to unstack */
}

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

/** Configure and fast check for LEDs. */
static void leds_configure(void)
{
    int i = PROBES_CNT;

    while (i--) {
        pinMode(PROBES[i].led, OUTPUT);
        digitalWrite(PROBES[i].led, HIGH);
    }

    /* Light up for a moment. */
    delay(400);

    i = PROBES_CNT;
    while (i--) {
        digitalWrite(PROBES[i].led, LOW);
    }

    DP("leds configured");
}

/** Illuminate assigned LEDs. */
static void leds_display(void)
{
    unsigned long tm = millis();
    byte SPIKE = tm % (200 * LED_SPIKE_DURATION) > LED_SPIKE_DURATION ? LOW : HIGH;
    byte BLINK = tm % (  2 * LED_BLINK_DURATION) > LED_BLINK_DURATION ? LOW : HIGH;
    byte FLASH = tm % (  2 * LED_FLASH_DURATION) > LED_FLASH_DURATION ? LOW : HIGH;

    int i = PROBES_CNT;

    while (i--) {
        byte sig =
                  PROBES[i].mode == LED_OFF     ? LOW
                : PROBES[i].mode == LED_SPIKE   ? SPIKE
                : PROBES[i].mode == LED_BLINK   ? BLINK
                : PROBES[i].mode == LED_ON      ? HIGH
                : PROBES[i].mode == LED_WARNING ? FLASH
                :                                 FLASH;

        digitalWrite(PROBES[i].led, sig);
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

        /* Skip completet or bad states. */
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
 * Force the valve to open.
 *
 * @param idx index of the valve from VALVES table
 */
static void valve_force_open(int idx)
{
    if (0 > idx || idx >= VALVES_CNT) {
        DP("bad valve specified, ignore");
        return;
    }

    VALVES[idx].act_state = VALVE_IGNORE;
    VALVES[idx].exp_state = VALVE_OPEN;
    VALVES[idx].elt = 0;

    digitalWrite(VALVES[idx].cport, IDLE_LVL);
    digitalWrite(VALVES[idx].oport, TRIG_LVL);
    delay(VALVE_FORCEOPENING_TIME);
    digitalWrite(VALVES[idx].oport, IDLE_LVL);

    VALVES[idx].act_state = VALVE_OPEN;
    VALVES[idx].exp_state = VALVE_OPEN;

    OVERALL_STATE = APP_MALFUNCTION != OVERALL_STATE ? APP_OK : OVERALL_STATE;
}

/**
 * Force the valve to close.
 *
 * @param idx index of the valve from VALVES table
 */
static void valve_force_close(int idx)
{
    if (0 > idx || idx >= VALVES_CNT) {
        DP("bad valve specified, ignore");
        return;
    }

    VALVES[idx].act_state = VALVE_IGNORE;
    VALVES[idx].exp_state = VALVE_CLOSE;
    VALVES[idx].elt = 0;

    digitalWrite(VALVES[idx].oport, IDLE_LVL);
    digitalWrite(VALVES[idx].cport, TRIG_LVL);
    delay(VALVE_FORCECLOSING_TIME);
    digitalWrite(VALVES[idx].cport, IDLE_LVL);

    VALVES[idx].act_state = VALVE_CLOSE;
    VALVES[idx].exp_state = VALVE_CLOSE;

    OVERALL_STATE = APP_MALFUNCTION != OVERALL_STATE ? APP_OK : OVERALL_STATE;
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

/** Perform reset by watchdog timer. */
static void reset(void)
{
    /* Ensure WD timer is power up. */
    wdt_enable(WDTO_4S);
    halt_on_error();
}

/** Startup procedure. */
void setup()
{
#ifdef DEBUG_PRINTOUT
    SWS.init(DEBUG_BAUD_RATE);
#endif
    DP("setup");

    peripheral_configure();
    web_configure();
    probes_configure();
    leds_configure();
    valves_configure();

    OVERALL_STATE = APP_OK;

    DP("ready to go...");
}

/** Main procedure. */
void loop()
{
    /* Application should restart once per month.
       Check this out. */
    unsigned long tm = millis();
    if (tm & 0x80000000) {
        DP("application life time expired");
        web_run();
        reset();
        return;
    }

    /* Check detectors. */
    probes_test();
    probes_check();
    probes_result();

    /* Control valves. */
    valves_solve();
    valves_run();
    valves_check();

    /* Display info. */
    leds_display();
    web_run();

#ifdef USE_POWER_SAVE
    /* Power save. */
    if (!Serial.available()) {
        enter_sleep(true, true);
    }
#else
    /* Debug stub. */
    delay(10);
#endif

}

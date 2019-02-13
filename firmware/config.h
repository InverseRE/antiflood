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



/*
 * Port mapping.
 */

#define WIFIRX    0                         /**< WiFi-shield RX */
#define WIFITX    1                         /**< WiFi-shield TX */
#define WIFIEN    2                         /**< enable WiFi-shield (active high) */
#define WIFIRS    3                         /**< reset WiFi-shield (active low) */

#define SW_RX     4                         /**< software serial rx (debug printout) */
#define SW_TX     5                         /**< software serial tx (debug printout) */

#define CLOSE     6                         /**< close valves */
#define OPEN      7                         /**< open valves */

#define LED0      8                         /**< LED 0 */
#define LED1      9                         /**< LED 1 */
#define LED2      10                        /**< LED 2 */
#define LED3      11                        /**< LED 3 */
#define LED4      12                        /**< LED 4 */
#define LED5      13                        /**< LED 5 */

#define PROBE0    A0                        /**< probe 0 */
#define PROBE1    A1                        /**< probe 1 */
#define PROBE2    A2                        /**< probe 2 */
#define PROBE3    A3                        /**< probe 3 */
#define PROBE4    A4                        /**< probe 4 */
#define PROBE5    A5                        /**< probe 5 */
#define VFOPST    A6                        /**< valve's forced state */
#define VCONSC    A7                        /**< valve supply current */



/*
 * Configures.
 */

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
#define TRIG_LVL                HIGH        /**< action engage level */
#define IDLE_LVL                LOW         /**< idle level */
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

/* LED settings. */
#define LED_SPIKE_DURATION      30          /**< spike on duration, ms */
#define LED_FLASH_DURATION      200         /**< rapid flashing on/off duration, ms */
#define LED_BLINK_DURATION      1000        /**< blink on/off duration, ms */

/* Overall firmware settings. */
#define AUX_SOUND                           /**< sound beeper */

/* Communication settings. */
#define APP_DEFAULT_IP 192, 168, 0, 111     /**< application default IP address */
#define APP_DEFAULT_IP_S "192.168.0.111"    /**< application default IP address string */
#define APP_DEFAULT_PORT 80                 /**< application default port */

/* Power save settings. */
#define USE_POWER_SAVE                      /**< use power saving mode between loop cycles */

/* Network interface settings. */
#define WIFI_STATION
#define WIFI_DEFAULT_SSID       ">)"        /**< default network identifier */
#define WIFI_DEFAULT_PASS       "00000000"  /**< default network access password */
#define WIFI_DEFAULT_CHAN       10          /**< default network channel number */
#define WIFI_DEFAULT_SECU       ENC_TYPE_WPA2_PSK

#define SHIELD_STARTUP_TIME     1000        /**< startup time for ESP8266, ms */
#define SHIELD_BAUD_RATE        115200      /**< shield's UART baud rate */
#define SHIELD_PWR_SAVE_ACTION  2           /**< turn off WIFI & activate power save mode action identifier */

#define WIFI_SSID               WIFI_DEFAULT_SSID
#define WIFI_PASS               WIFI_DEFAULT_PASS
#define WIFI_CHAN               WIFI_DEFAULT_CHAN
#define WIFI_SECU               WIFI_DEFAULT_SECU

/* Web-server settings. */
#define WEB_IP                  APP_DEFAULT_IP
#define WEB_IP_STR              APP_DEFAULT_IP_S
#define WEB_PORT                APP_DEFAULT_PORT
#define WEB_TRX_LATENCY         10          /**< some delays in web communication, ms */
#define WEB_IN_CACHE_SIZE       32          /**< size of input buffer for http */

